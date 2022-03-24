/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            updater.cpp
 *
 *  Mon Feb 28 10:00:00 UTC+1 2022
 *  Copyright 2022 Lars Bisballe
 *  larsbjensen@gmail.com
 ****************************************************************************/

/*
 *  This file is part of VisuTest.
 *
 *  VisuTest is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  VisuTest is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VisuTest; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include "updater.h"

#include <QVBoxLayout>
#include <QFile>
#include <QProcess>
#include <QMessageBox>
#include <QDirIterator>
#include <QLabel>
#include <QKeyEvent>
#include <QRadioButton>
#include <QEventLoop>

Updater::Updater(MainSettings &mainSettings, QWidget *parent)
  : QDialog(parent), mainSettings(mainSettings)
{
  setWindowTitle("VisuTest v" VERSION" updater");
  setFixedSize(1200, 700);
  
  QVBoxLayout *updatesLayout = new QVBoxLayout;
  updatesLayout->addWidget(new QLabel("<h3>Choose update / function:</h3>"), 0, Qt::AlignCenter);
  updatesButtons = new QButtonGroup;
  QList<QString> filters { "*.upd" };
  if(!QDir::homePath().isEmpty() && QDir::homePath().contains("/")) {
    // SEt %USER% so it can be used in the .upd files
    vars["%USER%"] = QDir::homePath().split("/").last();
    // SEt %HOME% so it can be used in the .upd files
    vars["%HOME%"] = QDir::homePath();
  }
  // SEt %WORKDIR% so it can be used in the .upd files
  vars["%WORKDIR%"] = QDir::currentPath();
  QDirIterator dirIt(mainSettings.updatesFolder,
                     filters,
                     QDir::Files | QDir::NoDotAndDotDot,
                     QDirIterator::Subdirectories);
  bool foundUpdate = false;
  while(dirIt.hasNext()) {
    dirIt.next();
    QFile updFile(dirIt.fileInfo().absoluteFilePath());
    if(updFile.open(QIODevice::ReadOnly)) {
      QString updTitle = "";
      QString updVersion = "";
      while(!updFile.atEnd()) {
        QString line = updFile.readLine().trimmed();
        if(line.contains(":")) {
          if(line.split(":").first() == "title") {
            updTitle = line.split(":").last();
            updTitle = varsReplace(updTitle);
          } else if(line.split(":").first() == "version") {
            updVersion = line.split(":").last();
            updVersion = varsReplace(updVersion);
          }
        }
      }
      if(!updTitle.isEmpty()) {
        QRadioButton *updateTypeButton = new QRadioButton((updVersion.isEmpty()?"":updVersion + ": ") + updTitle);
        if(!foundUpdate) {
          updateTypeButton->click();
        }
        updateTypeButton->setObjectName(dirIt.fileInfo().absoluteFilePath());
        updatesButtons->addButton(updateTypeButton);
        updatesLayout->addWidget(updateTypeButton);
        foundUpdate = true;
      }
      updFile.close();
    }
  }

  progressBar = new QProgressBar;
  progressBar->setFormat("%p% completed");
  progressBar->setAlignment(Qt::AlignCenter);
  updatesLayout->addWidget(progressBar);

  statusList = new QListWidget;
  statusList->setStyleSheet("QListWidget {background-color: black;}");
  updatesLayout->addWidget(statusList);

  if(!foundUpdate) {
    QLabel *noUpdate = new QLabel("<h2>No updates found in updates folder:</h2><h3>" + mainSettings.updatesFolder + "</h3>");
    updatesLayout->addWidget(noUpdate, 0, Qt::AlignCenter);
    
  }

  setLayout(updatesLayout);

  installEventFilter(this);
}

void Updater::applyUpdate(const QString &filename)
{
  abortUpdate = false;
  updateInProgress = true;
  progressBar->setFormat("%p% completed");
  progressBar->setValue(0);
  
  addStatus(STATUS, "Applying update from file '" + filename + "'");
  QFileInfo updInfo(filename);
  if(!updInfo.exists()) {
    QMessageBox::critical(this, "Error", "The update file '" + filename + "' does not exist. Can't update, aborting!");
    return;
  }

  updateSrcPath.clear();
  updateDstPath.clear();
  fileExcludes.clear();
  pathExcludes.clear();
  commands.clear();

  QFile commandFile(filename);
  if(commandFile.open(QIODevice::ReadOnly)) {
    while(!commandFile.atEnd()) {
      QString line = commandFile.readLine().trimmed();
      if(line.isEmpty() ||
         line.left(1) == "#" ||
         line.left(1) == ";" ||
         !line.contains(":")) {
        continue;
      }
      Command command;
      command.type = line.split(':').first().trimmed();
      command.parameters = line.split(':').last().split(';');
      commands.append(command);
    }
  }
  
  progressBar->setMaximum(commands.length());

  varsReplace();
  
  for(auto &command: commands) {
    if(abortUpdate) {
      break;
    }
    if(command.type == "aptinstall" && command.parameters.length()) {
      runCommand("sudo", { "apt-get", "-y", "update" });
      runCommand("sudo", (QList<QString> { "apt-get", "-y", "install" }) + command.parameters);
    } else if(command.type == "aptremove" && command.parameters.length()) {
      runCommand("sudo", { "apt-get", "-y", "update" });
      runCommand("sudo", (QList<QString> { "apt-get", "-y", "remove" }) + command.parameters);
    } else if(command.type == "fileexclude" && command.parameters.length() == 1) {
      addStatus(INFO, "Adding file exclude '" + command.parameters.at(0) + "'");
      fileExcludes.append(command.parameters.at(0));
    } else if(command.type == "pathexclude" && command.parameters.length() == 1) {
      addStatus(INFO, "Adding path exclude '" + command.parameters.at(0) + "'");
      pathExcludes.append(command.parameters.at(0));
    } else if(command.type == "setvar" && command.parameters.length() == 2) {
      QString variable = "%" + command.parameters.at(0) + "%";
      vars[variable] = command.parameters.at(1);
      addStatus(INFO, "Variable '" + variable + "' set to value '" + command.parameters.at(1) + "'");
      for(auto &replaceCommand: commands) {
        for(auto &parameter: replaceCommand.parameters) {
          parameter = varsReplace(parameter);
        }
      }
    } else if(command.type == "loadvars" && command.parameters.length() == 1) {
      addStatus(INFO, "Setting variables from file '" + command.parameters.at(0) + "'");
      QFile varsFile(command.parameters.at(0));
      if(varsFile.open(QIODevice::ReadOnly)) {
        while(!varsFile.atEnd()) {
          QByteArray line = varsFile.readLine().trimmed();
          if(line.contains("=")) {
            QString variable = "%" + line.split('=').first() + "%";
            QString value = line.split('=').last();
            vars[variable] = value;
            addStatus(INFO, "Variable '" + variable + "' set to value '" + value + "'");
          }
        }
        varsFile.close();
      }
      for(auto &replaceCommand: commands) {
        for(auto &parameter: replaceCommand.parameters) {
          parameter = varsReplace(parameter);
        }
      }
    } else if(command.type == "srcpath" && command.parameters.length() == 1) {
      QString tempPath = command.parameters.at(0);
      if(tempPath.right(1) == "/") {
        tempPath = tempPath.left(tempPath.length() - 1);
      }
      if(!updateDstPath.isEmpty() && tempPath.left(updateDstPath.length()) == updateDstPath) {
        addStatus(FATAL, "Source path is located beneath destination path. This is not allowed.");
      } else {
        addStatus(INFO, "Setting source path to '" + tempPath + "'");
        if(tempPath.left(1) != "/") {
          addStatus(FATAL, "Update source path '" + tempPath + "' is relative. This is not allowed.");
        } else {
          updateSrcPath = tempPath;
        }
      }
    } else if(command.type == "dstpath" && command.parameters.length() == 1) {
      QString tempPath = command.parameters.at(0);
      if(tempPath.right(1) == "/") {
        tempPath = tempPath.left(tempPath.length() - 1);
      }
      if(!updateSrcPath.isEmpty() && tempPath.left(updateSrcPath.length()) == updateSrcPath) {
        addStatus(FATAL, "Destination path is located beneath source path. This is not allowed.");
      } else {
        addStatus(INFO, "Setting destination path to '" + tempPath + "'");
        if(tempPath.left(1) != "/") {
          addStatus(FATAL, "Update destination path '" + tempPath + "' is relative. This is not allowed.");
        } else {
          updateDstPath = tempPath;
        }
      }
    } else if(command.type == "pretend" && command.parameters.length() == 1) {
      if(command.parameters.at(0) == "true") {
        pretend = true;
        addStatus(INFO, "Pretend set, no file changes will occur!");
      }
    } else if(command.type == "cpfile" &&
              (command.parameters.length() == 1 || command.parameters.length() == 2)) {
      if(command.parameters.first().left(1) != "/" && updateSrcPath.isEmpty()) {
        addStatus(FATAL, "Update source path undefined. 'srcpath=PATH' has to be set when using relative file paths with '" + command.type + "'");
      }
      if(command.parameters.last().left(1) != "/" && updateDstPath.isEmpty()) {
        addStatus(FATAL, "Update destination path undefined. 'dstpath=PATH' has to be set when using relative file paths with '" + command.type + "'");
      }
      cpFile(command);
    } else if(command.type == "cppath" &&
              command.parameters.length() == 1) {
      if(command.parameters.first().left(1) != "/" && updateSrcPath.isEmpty()) {
        addStatus(FATAL, "Update source path undefined. 'srcpath=PATH' has to be set when using relative paths with '" + command.type + "'");
      }
      if(command.parameters.last().left(1) != "/" && updateDstPath.isEmpty()) {
        addStatus(FATAL, "Update destination path undefined. 'dstpath=PATH' has to be set when using relative paths with '" + command.type + "'");
      }
      cpPath(command);
    } else if(command.type == "reboot" && command.parameters.length() == 1 && !pretend) {
      if(command.parameters.at(0) == "now") {
        addStatus(INFO, "Rebooting now...");
        QProcess::execute("reboot", {});
      }
    } else if(command.type == "message" && command.parameters.length() == 1) {
      addStatus(STATUS, command.parameters.at(0));
    }
    progressBar->setValue(progressBar->value() + 1);
  }
  progressBar->setFormat("All steps completed!");
  progressBar->setValue(progressBar->maximum());
  updateInProgress = false;
}

bool Updater::isExcluded(const QList<QString> &excludes, const QString &src)
{
  for(auto exclude: excludes) {
    if(exclude.left(1) != "/") {
      exclude.prepend(updateSrcPath + "/");
    }
    QFileInfo excludeInfo(exclude);
    if(src.left(excludeInfo.absoluteFilePath().length()) == excludeInfo.absoluteFilePath()) {
      return true;
    }
  }
  return false;
}

bool Updater::eventFilter(QObject *, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    if(updateInProgress) {
      return true;
    }
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent->key() == Qt::Key_Enter ||
       keyEvent->key() == Qt::Key_Return ||
       keyEvent->key() == Qt::Key_R) { // 'R' is the default enter key on the remote
      if(!updatesButtons->buttons().isEmpty()) {
        for(const auto *button: updatesButtons->buttons()) {
          if(button->isChecked()) {
            applyUpdate(button->objectName());
            break;
          }
        }
      }
      //accept();
      return true;
    }
  }
  return false;
}

void Updater::addStatus(const int &status, const QString &text)
{
  QListWidgetItem *item = new QListWidgetItem;
  if(status == INFO) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::green));
    item->setText((pretend?"INFO (pretend): ":"INFO: ") + text);
  } else if(status == STATUS) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::white));
    item->setText((pretend?"STATUS (pretend): ":"STATUS: ") + text);
  } else if(status == WARNING) {
    qWarning("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::yellow));
    item->setText((pretend?"WARNING (pretend): ":"WARNING: ") + text);
  } else if(status == FATAL) {
    qCritical("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::red));
    item->setText((pretend?"FATAL (pretend): ":"FATAL: ") + text);
    abortUpdate = true;
    updateInProgress = false;
  }
  statusList->addItem(item);
  statusList->scrollToBottom();
  QEventLoop waiter;
  QTimer::singleShot(10, &waiter, &QEventLoop::quit);
  waiter.exec();
}

bool Updater::cpFile(Command &command)
{
  if(abortUpdate) {
    return false;
  }
  QString srcFileString = command.parameters.first();
  if(srcFileString.left(1) != "/") {
    srcFileString.prepend(updateSrcPath + "/");
  }
  QFileInfo srcInfo(srcFileString);

  QString dstFileString = command.parameters.last();
  if(dstFileString.left(1) != "/") {
    dstFileString.prepend(updateDstPath + "/");
  }

  QFileInfo dstInfo(dstFileString);
  
  addStatus(INFO, "Copying file '" + srcInfo.absoluteFilePath() + "' to '" + dstInfo.absoluteFilePath() + "'");

  if(isExcluded(fileExcludes, srcInfo.absoluteFilePath())) {
    addStatus(WARNING, "Source file marked for exclusion, continuing without copying!");
    return true;
  }

  if(!QFile::exists(srcInfo.absoluteFilePath())) {
    addStatus(FATAL, "Source file not found!");
    return false;
  }

  if(!pretend && !QDir::root().mkpath(dstInfo.absolutePath())) {
    addStatus(FATAL, "Path '" + dstInfo.absolutePath() + "' for file could not be created!");
    return false;
  }

  if(QFile::exists(dstInfo.absoluteFilePath())) {
    if(!pretend && !QFile::remove(dstInfo.absoluteFilePath())) {
      addStatus(FATAL, "Destination file could not be removed before copying!");
      return false;
    }
  }
  if(!pretend && !QFile::copy(srcInfo.absoluteFilePath(), dstInfo.absoluteFilePath())) {
    addStatus(FATAL, "File copy failed!");
    return false;
  }
  addStatus(INFO, "File copy successful!");
  return true;
}

bool Updater::cpPath(Command &command)
{
  if(abortUpdate) {
    return false;
  }
  QString srcDirString = command.parameters.at(0);
  if(srcDirString.left(1) != "/") {
    srcDirString.prepend(updateSrcPath + "/");
  }
  QDir srcDir(srcDirString);

  QString dstDirString = command.parameters.at(0);
  if(dstDirString.left(1) != "/") {
    dstDirString.prepend(updateDstPath + "/");
  }
  QDir dstDir(dstDirString);

  addStatus(INFO, "Copying path '" + srcDir.absolutePath() + "' to '" + dstDir.absolutePath() + "'");

  if(!srcDir.exists()) {
    addStatus(FATAL, "Source file path does not exist!");
    return false;
  }
  if(!pretend && !QDir::root().mkpath(dstDir.absolutePath())) {
    addStatus(FATAL, "Path '" + dstDir.absolutePath() + "' could not be created");
    return false;
  }

  QDirIterator dirIt(srcDir.absolutePath(),
                     {"*"},
                     QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                     QDirIterator::Subdirectories);
  while(dirIt.hasNext()) {
    dirIt.next();
    QFileInfo itSrc = dirIt.fileInfo();
    QFileInfo itDst(dstDir.absolutePath() + "/" + itSrc.absoluteFilePath().mid(srcDir.absolutePath().length()));
    if(itSrc.isDir()) {
      addStatus(INFO, "Copying path '" + itSrc.absoluteFilePath() + "' to '" + itDst.absoluteFilePath() + "'");
      if(isExcluded(pathExcludes, itSrc.absoluteFilePath())) {
        addStatus(WARNING, "Source path marked for exclusion, continuing without copying!");
        continue;
      }
      if(!pretend && !QDir::root().mkpath(itDst.absoluteFilePath())) {
        addStatus(FATAL, "Path '" + itDst.absoluteFilePath() + "' could not be created");
        return false;
      }
    } else if(itSrc.isFile()) {
      Command tempCommand;
      tempCommand.type = "cpfile";
      tempCommand.parameters = QList<QString> { itSrc.absoluteFilePath(), itDst.absoluteFilePath() };
      cpFile(tempCommand);
    }
  }
  return true;
}

bool Updater::runCommand(const QString &program, const QList<QString> &args, const bool &critical)
{
  QString fullCommand = program + " ";
  for(const auto &arg: args) {
    fullCommand.append(arg + " ");
  }
  fullCommand = fullCommand.trimmed();
  
  addStatus(STATUS, "Running terminal command: '" + fullCommand + "', please wait...");
  if(!pretend) {
    QProcess aptProc;
    aptProc.setProgram(program);
    aptProc.setArguments(args);
    aptProc.start();
    aptProc.waitForFinished(10 * 60 * 1000); // 10 minutes
    if(aptProc.exitStatus() != QProcess::NormalExit && aptProc.exitCode() != 0) {
      addStatus(WARNING, "Terminal command failed with the following output:\n");
      addStatus(WARNING, aptProc.readAllStandardOutput().data());
      if(critical) {
        addStatus(FATAL, aptProc.readAllStandardError().data());
      } else {
        addStatus(WARNING, aptProc.readAllStandardError().data());
      }
      return false;
    }
    
    addStatus(INFO, "Terminal command completed successfully with the following output:");
    addStatus(INFO, aptProc.readAllStandardOutput().data());
    addStatus(INFO, aptProc.readAllStandardError().data());
  }

  return true;
}

QString Updater::varsReplace(QString string)
{
  for(int a = 0; a < vars.keys().length(); ++a) {
    if(!string.isEmpty()) {
      string.replace(vars.keys().at(a), vars[vars.keys().at(a)]);
    } else {
      for(auto &replaceCommand: commands) {
        for(auto &parameter: replaceCommand.parameters) {
          parameter.replace(vars.keys().at(a), vars[vars.keys().at(a)]);
        }
      }
    }
  }
  return string;
}
