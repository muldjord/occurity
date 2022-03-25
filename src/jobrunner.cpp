/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jobrunner.cpp
 *
 *  Mon Feb 28 10:00:00 UTC+1 2022
 *  Copyright 2022 Lars Bisballe
 *  larsbjensen@gmail.com
 ****************************************************************************/

/*
 *  This file is part of Occurity.
 *
 *  Occurity is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Occurity is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Occurity; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include "jobrunner.h"
#include "messagebox.h"

#include <QVBoxLayout>
#include <QFile>
#include <QProcess>
#include <QMessageBox>
#include <QDirIterator>
#include <QLabel>
#include <QKeyEvent>
#include <QRadioButton>
#include <QEventLoop>

JobRunner::JobRunner(MainSettings &mainSettings, QWidget *parent)
  : QDialog(parent), mainSettings(mainSettings)
{
  setWindowTitle("Occurity v" VERSION" jobrunner");
  setFixedSize(1200, 700);
  
  QVBoxLayout *jobsLayout = new QVBoxLayout;
  jobsLayout->addWidget(new QLabel("<h3>Choose job / function:</h3>"), 0, Qt::AlignCenter);
  jobsButtons = new QButtonGroup;
  QList<QString> filters { "*.job" };
  if(!QDir::homePath().isEmpty() && QDir::homePath().contains("/")) {
    // SEt %USER% so it can be used in the .job files
    vars["%USER%"] = QDir::homePath().split("/").last();
    // SEt %HOME% so it can be used in the .job files
    vars["%HOME%"] = QDir::homePath();
  }
  // SEt %WORKDIR% so it can be used in the .job files
  vars["%WORKDIR%"] = QDir::currentPath();
  QDirIterator dirIt(mainSettings.jobsFolder,
                     filters,
                     QDir::Files | QDir::NoDotAndDotDot,
                     QDirIterator::Subdirectories);
  bool foundJob = false;
  setStyleSheet("QLabel {font-weight:bold;}");
      QMap<QString, QVBoxLayout*> categories;
  while(dirIt.hasNext()) {
    dirIt.next();
    QFile jobFile(dirIt.fileInfo().absoluteFilePath());
    if(jobFile.open(QIODevice::ReadOnly)) {
      QString jobTitle = "";
      QString jobVersion = "";
      QString category = "Uncategorized";
      while(!jobFile.atEnd()) {
        QString line = jobFile.readLine().trimmed();
        if(line.contains(":")) {
          if(line.split(":").first() == "title") {
            jobTitle = line.split(":").last();
            jobTitle = varsReplace(jobTitle);
          } else if(line.split(":").first() == "version") {
            jobVersion = line.split(":").last();
            jobVersion = varsReplace(jobVersion);
          } else if(line.split(":").first() == "category") {
            category = line.split(":").last();
            category = varsReplace(category);
          }
        }
      }
      if(!jobTitle.isEmpty()) {
        if(!categories.contains(category)) {
          printf("Creating category '%s'\n", category.toStdString().c_str());
          categories[category] = new QVBoxLayout;
          categories[category]->addWidget(new QLabel(category));
          jobsLayout->addLayout(categories[category]);
        }
        QRadioButton *jobTypeButton = new QRadioButton((jobVersion.isEmpty()?"":jobVersion + ": ") + jobTitle);
        if(!foundJob) {
          jobTypeButton->click();
        }
        jobTypeButton->setObjectName(dirIt.fileInfo().absoluteFilePath());
        jobsButtons->addButton(jobTypeButton);
        categories[category]->addWidget(jobTypeButton);
        foundJob = true;
      }
      jobFile.close();
    }
  }

  progressBar = new QProgressBar;
  progressBar->setFormat("%p% completed");
  progressBar->setAlignment(Qt::AlignCenter);
  jobsLayout->addWidget(progressBar);

  statusList = new QListWidget;
  statusList->setStyleSheet("QListWidget {background-color: black;}");
  jobsLayout->addWidget(statusList);

  if(!foundJob) {
    QLabel *noJob = new QLabel("<h2>No jobs found in jobs folder:</h2><h3>" + mainSettings.jobsFolder + "</h3>");
    jobsLayout->addWidget(noJob, 0, Qt::AlignCenter);
    
  }

  setLayout(jobsLayout);

  installEventFilter(this);
}

void JobRunner::runJob(const QString &filename)
{
  abortJob = false;
  jobInProgress = true;
  progressBar->setFormat("%p% completed");
  progressBar->setValue(0);
  
  addStatus(STATUS, "Running script from file '" + filename + "'");
  QFileInfo jobInfo(filename);
  if(!jobInfo.exists()) {
    MessageBox messageBox(QMessageBox::Critical, "Error", "The job file '" + filename + "' does not exist. Can't run, aborting!", QMessageBox::Ok, this);
    messageBox.exec();
    return;
  }

  jobSrcPath.clear();
  jobDstPath.clear();
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
    if(abortJob) {
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
      if(!jobDstPath.isEmpty() && tempPath.left(jobDstPath.length()) == jobDstPath) {
        addStatus(FATAL, "Source path is located beneath destination path. This is not allowed.");
      } else {
        addStatus(INFO, "Setting source path to '" + tempPath + "'");
        if(tempPath.left(1) != "/") {
          addStatus(FATAL, "Job source path '" + tempPath + "' is relative. This is not allowed.");
        } else {
          jobSrcPath = tempPath;
        }
      }
    } else if(command.type == "dstpath" && command.parameters.length() == 1) {
      QString tempPath = command.parameters.at(0);
      if(tempPath.right(1) == "/") {
        tempPath = tempPath.left(tempPath.length() - 1);
      }
      if(!jobSrcPath.isEmpty() && tempPath.left(jobSrcPath.length()) == jobSrcPath) {
        addStatus(FATAL, "Destination path is located beneath source path. This is not allowed.");
      } else {
        addStatus(INFO, "Setting destination path to '" + tempPath + "'");
        if(tempPath.left(1) != "/") {
          addStatus(FATAL, "Job destination path '" + tempPath + "' is relative. This is not allowed.");
        } else {
          jobDstPath = tempPath;
        }
      }
    } else if(command.type == "pretend" && command.parameters.length() == 1) {
      if(command.parameters.at(0) == "true") {
        pretend = true;
        addStatus(INFO, "Pretend set, no file changes will occur!");
      }
    } else if(command.type == "cpfile" &&
              (command.parameters.length() == 1 || command.parameters.length() == 2)) {
      if(command.parameters.first().left(1) != "/" && jobSrcPath.isEmpty()) {
        addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative file paths with '" + command.type + "'");
      }
      if(command.parameters.last().left(1) != "/" && jobDstPath.isEmpty()) {
        addStatus(FATAL, "Job destination path undefined. 'dstpath=PATH' has to be set when using relative file paths with '" + command.type + "'");
      }
      cpFile(command);
    } else if(command.type == "rmfile" && command.parameters.length() == 1) {
      addStatus(INFO, "Removing file '" + command.parameters.first() + "'");
      if(command.parameters.first().left(1) != "/") {
        addStatus(FATAL, "'" + command.type + "' command requires a non-relative file path. File not removed");
        continue;
      }
      if(!QFile::exists(command.parameters.first())) {
        addStatus(FATAL, "File doesn't exist!");
        continue;
      }
      if(QFile::remove(command.parameters.first())) {
        addStatus(INFO, "File removed!");
      } else {
        addStatus(WARNING, "File could not be removed. Maybe a permission problem?");
      }
    } else if(command.type == "cppath" &&
              command.parameters.length() == 1) {
      if(command.parameters.first().left(1) != "/" && jobSrcPath.isEmpty()) {
        addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative paths with '" + command.type + "'");
      }
      if(command.parameters.last().left(1) != "/" && jobDstPath.isEmpty()) {
        addStatus(FATAL, "Job destination path undefined. 'dstpath=PATH' has to be set when using relative paths with '" + command.type + "'");
      }
      cpPath(command);
    } else if(command.type == "reboot" && command.parameters.length() == 1) {
      if(command.parameters.at(0) == "now") {
        addStatus(INFO, "Forcing reboot now...");
        QProcess::execute("reboot", {});
      } else if(command.parameters.at(0) == "ask") {
        MessageBox messageBox(QMessageBox::Question, "Reboot?", "Do you wish to perform a system reboot?", QMessageBox::Yes | QMessageBox::No, this);
        messageBox.exec();
        if(messageBox.result() == QDialog::Accepted) {
          addStatus(INFO, "User requested reboot, rebooting now...");
          QProcess::execute("reboot", {});
        } else {
          addStatus(INFO, "User cancelled reboot.");
        }
      }
    } else if(command.type == "shutdown" && command.parameters.length() == 1) {
      if(command.parameters.at(0) == "now") {
        addStatus(INFO, "Forcing system shutdown now...");
        QProcess::execute("halt", {});
      } else if(command.parameters.at(0) == "ask") {
        MessageBox messageBox(QMessageBox::Question, "Shutdown?", "Do you wish to perform a system shutdown?", QMessageBox::Yes | QMessageBox::No, this);
        messageBox.exec();
        if(messageBox.result() == QDialog::Accepted) {
          addStatus(INFO, "User requested shutdown, shutting down system now...");
          QProcess::execute("halt", {});
        } else {
          addStatus(INFO, "User cancelled shutdown.");
        }
      }
    } else if(command.type == "message" && command.parameters.length() == 1) {
      addStatus(STATUS, command.parameters.at(0));
    }
    progressBar->setValue(progressBar->value() + 1);
  }
  progressBar->setFormat("All steps completed!");
  progressBar->setValue(progressBar->maximum());
  jobInProgress = false;
}

bool JobRunner::isExcluded(const QList<QString> &excludes, const QString &src)
{
  for(auto exclude: excludes) {
    if(exclude.left(1) != "/") {
      exclude.prepend(jobSrcPath + "/");
    }
    QFileInfo excludeInfo(exclude);
    if(src.left(excludeInfo.absoluteFilePath().length()) == excludeInfo.absoluteFilePath()) {
      return true;
    }
  }
  return false;
}

bool JobRunner::eventFilter(QObject *, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    if(jobInProgress) {
      return true;
    }
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent->key() == Qt::Key_Enter ||
       keyEvent->key() == Qt::Key_Return ||
       keyEvent->key() == Qt::Key_R) { // 'R' is the default enter key on the remote
      if(!jobsButtons->buttons().isEmpty()) {
        for(const auto *button: jobsButtons->buttons()) {
          if(button->isChecked()) {
            runJob(button->objectName());
            break;
          }
        }
      }
      return true;
    } else if(keyEvent->key() == Qt::Key_J) {
      accept();
      return true;
    }
  }
  return false;
}

void JobRunner::addStatus(const int &status, const QString &text)
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
    abortJob = true;
    jobInProgress = false;
  }
  statusList->addItem(item);
  statusList->scrollToBottom();
  QEventLoop waiter;
  QTimer::singleShot(10, &waiter, &QEventLoop::quit);
  waiter.exec();
}

bool JobRunner::cpFile(Command &command)
{
  if(abortJob) {
    return false;
  }
  QString srcFileString = command.parameters.first();
  if(srcFileString.left(1) != "/") {
    srcFileString.prepend(jobSrcPath + "/");
  }
  QFileInfo srcInfo(srcFileString);

  QString dstFileString = command.parameters.last();
  if(dstFileString.left(1) != "/") {
    dstFileString.prepend(jobDstPath + "/");
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

bool JobRunner::cpPath(Command &command)
{
  if(abortJob) {
    return false;
  }
  QString srcDirString = command.parameters.at(0);
  if(srcDirString.left(1) != "/") {
    srcDirString.prepend(jobSrcPath + "/");
  }
  QDir srcDir(srcDirString);

  QString dstDirString = command.parameters.at(0);
  if(dstDirString.left(1) != "/") {
    dstDirString.prepend(jobDstPath + "/");
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

bool JobRunner::runCommand(const QString &program, const QList<QString> &args, const bool &critical)
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

QString JobRunner::varsReplace(QString string)
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
