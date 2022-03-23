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
  // SEt %HOME% so it can be used in the .upd files
  vars["%HOME%"] = QDir::homePath();
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
  addStatus(STATUS, "Applying update from file '" + filename + "'");
  QFileInfo updInfo(filename);
  if(!updInfo.exists()) {
    QMessageBox::critical(this, "Error", "The update file '" + filename + "' does not exist. Can't update, aborting!");
    return;
  }
  QList<Command> commands;
  fileExcludes.clear();
  pathExcludes.clear();
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

  for(auto &replaceCommand: commands) {
    for(auto &parameter: replaceCommand.parameters) {
      parameter = varsReplace(parameter);
      printf("PARAMETER: '%s'\n", parameter.toStdString().c_str());
    }
  }
  
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
    } else if(command.type == "setvars" && command.parameters.length() == 1) {
      addStatus(INFO, "Setting variables from file '" + command.parameters.at(0) + "'");
      QFile varsFile(command.parameters.at(0));
      if(varsFile.open(QIODevice::ReadOnly)) {
        while(!varsFile.atEnd()) {
          QByteArray line = varsFile.readLine().trimmed();
          if(line.contains("=")) {
            QString variable = "%" + line.split('=').first() + "%";
            QString value = line.split('=').last();
            vars[variable] = value;
            addStatus(INFO, "'" + variable + "' set to '" + value + "'");
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
      addStatus(INFO, "Setting source path to '" + command.parameters.at(0) + "'");
      mainSettings.updateSrcPath = command.parameters.at(0);
    } else if(command.type == "dstpath" && command.parameters.length() == 1) {
      if(command.parameters.at(0).left(mainSettings.updateSrcPath.length()) ==
         mainSettings.updateSrcPath) {
        addStatus(FATAL, "Destination path is locatated beneath source path. This is not allowed.");
      } else {
        addStatus(INFO, "Setting destination path to '" + command.parameters.at(0) + "'");
        mainSettings.updateDstPath = command.parameters.at(0);
      }
    } else if(command.type == "pretend" && command.parameters.length() == 1) {
      if(command.parameters.at(0) == "true") {
        pretend = true;
        addStatus(INFO, "Pretend set, no file changes will occur!");
      }
    } else if(command.type == "cpfile" &&
              (command.parameters.length() == 1 || command.parameters.length() == 2)) {
      cpFile(command);
    } else if(command.type == "cppath" &&
              command.parameters.length() == 1) {
      cpPath(command);
    } else if(command.type == "reboot" && command.parameters.length() == 1) {
      if(!abortUpdate && !pretend) {
        if(command.parameters.at(0) == "now") {
          addStatus(INFO, "Rebooting now...");
          QProcess::execute("reboot", {});
        }
      }
    } else if(command.type == "message" && command.parameters.length() == 1) {
      addStatus(STATUS, command.parameters.at(0));
    }
    progressBar->setValue(progressBar->value() + 1);
  }
  progressBar->setFormat("All steps completed!");
  progressBar->setValue(progressBar->maximum());
}

bool Updater::isExcluded(const QList<QString> &excludes, const QString &src)
{
  for(const auto &exclude: excludes) {
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
    item->setText((pretend?"INFO PRETEND: ":"INFO: ") + text);
  } else if(status == STATUS) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::white));
    item->setText((pretend?"STATUS PRETEND: ":"STATUS: ") + text);
  } else if(status == WARNING) {
    qWarning("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::yellow));
    item->setText((pretend?"WARNING PRETEND: ":"WARNING: ") + text);
  } else if(status == FATAL) {
    qCritical("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::red));
    item->setText((pretend?"FATAL PRETEND: ":"FATAL: ") + text);
    abortUpdate = true;
  }
  statusList->addItem(item);
  statusList->scrollToBottom();
  QEventLoop waiter;
  QTimer::singleShot(20, &waiter, &QEventLoop::quit);
  waiter.exec();
}

bool Updater::cpFile(Command &command)
{
  QFileInfo src((command.parameters.at(0).left(1) == "/"?command.parameters.at(0):mainSettings.updateSrcPath + (mainSettings.updateSrcPath.right(1) == "/"?"":"/") + command.parameters.at(0)));
  QFileInfo dst((command.parameters.last().left(1) == "/"?command.parameters.last():mainSettings.updateDstPath + (mainSettings.updateDstPath.right(1) == "/"?"":"/") + command.parameters.last()));
  
  addStatus(INFO, "Copying file '" + src.absoluteFilePath() + "' to '" + dst.absoluteFilePath() + "'");

  if(isExcluded(fileExcludes, src.absoluteFilePath())) {
    addStatus(INFO, "File marked for exclusion, copy aborted.");
    return true;
  }

  if(!QFile::exists(src.absoluteFilePath())) {
    addStatus(FATAL, "Source file not found!");
    return false;
  }

  if(!pretend && !QDir::root().mkpath(dst.absolutePath())) {
    addStatus(FATAL, "Path '" + dst.absolutePath() + "' for file could not be created!");
    return false;
  }

  if(QFile::exists(dst.absoluteFilePath())) {
    if(!pretend && !QFile::remove(dst.absoluteFilePath())) {
      addStatus(FATAL, "Destination file could not be removed before copying!");
      return false;
    }
  }
  if(!pretend && !QFile::copy(src.absoluteFilePath(), dst.absoluteFilePath())) {
    addStatus(FATAL, "File copy failed!");
    return false;
  }
  addStatus(INFO, "File copy successful!");
  return true;
}

bool Updater::cpPath(Command &command)
{
  QDir srcDir((command.parameters.at(0).left(1) == "/"?command.parameters.at(0):mainSettings.updateSrcPath + (mainSettings.updateSrcPath.right(1) == "/"?"":"/") + command.parameters.at(0)));
  QDir dstDir((command.parameters.last().left(1) == "/"?command.parameters.last():mainSettings.updateDstPath + (mainSettings.updateDstPath.right(1) == "/"?"":"/") + command.parameters.last()));

  addStatus(INFO, "Copying path '" + srcDir.absolutePath() + "' to '" + dstDir.absolutePath() + "'");

  if(!srcDir.exists()) {
    addStatus(FATAL, "Source path does not exist!");
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
    if(isExcluded(fileExcludes, itSrc.absoluteFilePath())) {
      addStatus(INFO, "File marked for exclusion, continuing without copying!");
      return true;
    }
    if(isExcluded(pathExcludes, itSrc.absoluteFilePath())) {
      addStatus(INFO, "Path marked for exclusion, continuing without copying!");
      return true;
    }
    if(itSrc.isDir()) {
      if(!pretend && !QDir::root().mkpath(itDst.absolutePath())) {
        addStatus(FATAL, "Path '" + itDst.absolutePath() + "' could not be created");
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
  
  addStatus(INFO, "Running terminal command: '" + fullCommand + "'");
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
    string.replace(vars.keys().at(a), vars[vars.keys().at(a)]);
  }
  return string;
}
