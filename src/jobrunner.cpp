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
  setFixedSize(1200, 800);

  setStyleSheet("QLabel {font-weight:bold;} QRadioButton {margin-left: 10px;}");
  
  jobButtons = new QButtonGroup;

  setHardcodedVars();

  QList<QVBoxLayout*> categoryLayouts;
  QList<QRadioButton *> jobButtonsList;
  QDir jobDir(mainSettings.jobsFolder, "*.job", QDir::Name, QDir::Files | QDir::NoDotAndDotDot);
  for(const auto &jobInfo: jobDir.entryInfoList()) {
    QFile jobFile(jobInfo.absoluteFilePath());
    if(jobFile.open(QIODevice::ReadOnly)) {
      QString jobTitle = "";
      QString jobVersion = "";
      QString categoryString = "Uncategorized";
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
            categoryString = line.split(":").last();
            categoryString = varsReplace(categoryString);
          }
        }
      }
      if(!jobTitle.isEmpty()) {
        QVBoxLayout *categoryLayout = nullptr;
        for(auto *existingCategoryLayout: categoryLayouts) {
          if(existingCategoryLayout->objectName() == categoryString) {
            categoryLayout = existingCategoryLayout;
            break;
          }
        }
        if(categoryLayout == nullptr) {
          printf("Creating category '%s'\n", categoryString.toStdString().c_str());
          categoryLayout = new QVBoxLayout;
          categoryLayout->setObjectName(categoryString);
          categoryLayout->addWidget(new QLabel(categoryString));
          categoryLayouts.append(categoryLayout);
        }
        QRadioButton *jobTypeButton = new QRadioButton((jobVersion.isEmpty()?"":jobVersion + ": ") + jobTitle);
        jobTypeButton->setObjectName(jobInfo.absoluteFilePath());
        jobTypeButton->setAccessibleName(categoryString);
        jobButtons->addButton(jobTypeButton);
        jobButtonsList.append(jobTypeButton);
      }
      jobFile.close();
    }
  }
  std::sort(jobButtonsList.begin(), jobButtonsList.end(),	[](const QRadioButton *a, const QRadioButton *b) -> bool { return a->objectName().toLower() < b->objectName().toLower();});

  std::sort(categoryLayouts.begin(), categoryLayouts.end(),	[](const QVBoxLayout *a, const QVBoxLayout *b) -> bool { return a->objectName().toLower() < b->objectName().toLower();});    

  bool firstClicked = false;
  for(auto *categoryLayout: categoryLayouts) {
    for(auto *jobButton: jobButtonsList) {
      if(jobButton->accessibleName() == categoryLayout->objectName()) {
        if(!firstClicked) {
          jobButton->click();
          firstClicked = true;
        }
        categoryLayout->addWidget(jobButton);
      }
    }
  }
  /*
    QList<QPair<QString, QList<QVBoxLayout *> > categoryLayouts;
  for(const auto &key: categoryLayouts.keys()) {
    categoryLayouts.append(QPair<QString, QVBoxLayout *>(key, categoryLayouts[key]));
    QObjectList jobButtonsLists = categoryLayouts[key]->children();
    categoryLayouts[key]->clear();
    for(auto &jobButtonsList: jobButtonsLists) {
      categoryLayouts[key]->addWidget(reinterpret_cast<QRadioButton*>(jobButtonsList));
    }
  }

  std::sort(categoryLayouts.begin(), categoryLayouts.end(),	[](const QPair<QString, QVBoxLayout *> a, const QPair<QString, QVBoxLayout *> b) -> bool { return a.first.toLower() < b.first.toLower();});
    */

  QVBoxLayout *jobsLayout = new QVBoxLayout;

  if(!firstClicked) {
    QLabel *noJob = new QLabel("<h2>No jobs found in jobs folder:</h2><h3>" + mainSettings.jobsFolder + "</h3>");
    jobsLayout->addWidget(noJob, 2, Qt::AlignCenter);
    
  } else {
    jobsLayout->addWidget(new QLabel("<h3>Choose job to run:</h3>"), 0, Qt::AlignCenter);
    for(auto *categoryLayout: categoryLayouts) {
      jobsLayout->addLayout(categoryLayout);
    }
    progressBar = new QProgressBar;
    progressBar->setFormat("%p% completed");
    progressBar->setAlignment(Qt::AlignCenter);
    jobsLayout->addWidget(progressBar);
    
    statusList = new QListWidget;
    statusList->setWordWrap(true);
    statusList->setStyleSheet("QListWidget {background-color: black;}");
    jobsLayout->addWidget(statusList);
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
  
  statusList->clear();

  vars.clear();
  setHardcodedVars();

  jobSrcPath.clear();
  jobDstPath.clear();

  fileExcludes.clear();
  pathExcludes.clear();

  commands.clear();

  pretend = false;

  addStatus(STATUS, "Running script from file '" + filename + "'");

  QFileInfo jobInfo(filename);
  if(!jobInfo.exists()) {
    MessageBox messageBox(QMessageBox::Critical, "Error", "The job file '" + filename + "' does not exist. Can't run, aborting!", QMessageBox::Ok, this);
    messageBox.exec();
    return;
  }
  
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
      if(command.type == "title" ||
         command.type == "version" ||
         command.type == "category") {
        continue;
      }
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

    // Add entire command string to debug output
    if(command.type != "message") {
      addStatus(COMMAND, getCommandString(command));
    }
    
    if(command.type == "aptinstall") {
      if(command.parameters.length() == 1) {
        runCommand("sudo", { "apt-get", "-y", "update" });
        runCommand("sudo", (QList<QString> { "apt-get", "-y", "install" }) + command.parameters);
      }

    } else if(command.type == "aptremove") {
      if(command.parameters.length() == 1) {
        runCommand("sudo", { "apt-get", "-y", "update" });
        runCommand("sudo", (QList<QString> { "apt-get", "-y", "remove" }) + command.parameters);
      }

    } else if(command.type == "fileexclude") {
      if(command.parameters.length() == 1) {
        fileExclude(command.parameters.at(0));
      }

    } else if(command.type == "pathexclude") {
      if(command.parameters.length() == 1) {
        pathExclude(command.parameters.at(0));
      }

    } else if(command.type == "setvar") {
      if(command.parameters.length() == 2) {
        setVar(command.parameters.at(0), command.parameters.at(1));
      }

    } else if(command.type == "loadvars") {
      if(command.parameters.length() == 1) {
        loadVars(command.parameters.at(0));
      }

    } else if(command.type == "srcpath") {
      if(command.parameters.length() == 1) {
        srcPath(command.parameters.at(0));
      }

    } else if(command.type == "dstpath") {
      if(command.parameters.length() == 1) {
        dstPath(command.parameters.at(0));
      }

    } else if(command.type == "rmpath") {
      if((command.parameters.length() == 1 || command.parameters.length() == 2)) {
        rmPath(command.parameters.at(0), (command.parameters.length() == 2 && command.parameters.at(1) == "ask"?true:false));
      }

    } else if(command.type == "pretend") {
      if(command.parameters.length() == 1) {
        if(command.parameters.at(0) == "true") {
          addStatus(STATUS, "Setting pretend! No path or file changes will occur while processing this job.");
          pretend = true;
        }
      }

    } else if(command.type == "cpfile") {
      if(command.parameters.length() == 1 || command.parameters.length() == 2) {
        cpFile(command.parameters.first(), command.parameters.last());
      }

    } else if(command.type == "rmfile") {
      if(command.parameters.length() == 1) {
        rmFile(command.parameters.at(0));
      }

    } else if(command.type == "cppath") {
      if(command.parameters.length() == 1 || command.parameters.length() == 2) {
        cpPath(command.parameters.at(0), command.parameters.last());
      }

    } else if(command.type == "reboot") {
      if(command.parameters.length() == 1) {
        reboot(command.parameters.at(0));
      }

    } else if(command.type == "shutdown") {
      if(command.parameters.length() == 1) {
        shutdown(command.parameters.at(0));
      }

    } else if(command.type == "message") {
      if(command.parameters.length() == 1) {
        addStatus(STATUS, command.parameters.at(0));
      }

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
      if(!jobButtons->buttons().isEmpty()) {
        for(const auto *jobButton: jobButtons->buttons()) {
          if(jobButton->isChecked()) {
            runJob(jobButton->objectName());
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
  QFont itemFont("monospace");
  int delay = 5;
  item->setFont(itemFont);
  if(status == INFO) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::green));
    item->setText(QString("  ") + (pretend?"pretend: ":"") + text);
    //delay = 10;
  } else if(status == STATUS) {
    qInfo("%s", text.toUtf8().data());
    itemFont.setPixelSize(20);
    item->setFont(itemFont);
    item->setForeground(QBrush(Qt::white));
    item->setText((pretend?"pretend: ":"") + text);
    //delay = 250;
  } else if(status == WARNING) {
    qWarning("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::yellow));
    item->setText(QString("    ") + (pretend?"pretend: ":"") + text);
    //delay = 500;
  } else if(status == FATAL) {
    qCritical("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::red));
    item->setText(QString("    ") + (pretend?"pretend: ":"") + text);
    abortJob = true;
    jobInProgress = false;
  } else if(status == COMMAND) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::gray));
    item->setText((pretend?"pretend: ":"") + text);
  }
  statusList->addItem(item);
  statusList->scrollToBottom();
  QEventLoop waiter;
  QTimer::singleShot(delay, &waiter, &QEventLoop::quit);
  waiter.exec();
}

bool JobRunner::cpFile(const QString &srcFile, const QString &dstFile)
{
  if(abortJob) {
    return false;
  }

  if(srcFile.left(1) != "/" && jobSrcPath.isEmpty()) {
    addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative file paths with 'cpfile'");
    return false;
  }
  
  if(dstFile.left(1) != "/" && jobDstPath.isEmpty()) {
    addStatus(FATAL, "Job destination path undefined. 'dstpath=PATH' has to be set when using relative file paths with 'cpfile'");
    return false;
  }

  QString srcFileString = srcFile;
  if(srcFileString.left(1) != "/") {
    srcFileString.prepend(jobSrcPath + "/");
  }
  QFileInfo srcInfo(srcFileString);

  QString dstFileString = dstFile;
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
    addStatus(FATAL, "Path '" + dstInfo.absolutePath() + "' could not be created!");
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

bool JobRunner::cpPath(const QString &srcPath, const QString &dstPath)
{
  if(abortJob) {
    return false;
  }

  if(srcPath.left(1) != "/" && jobSrcPath.isEmpty()) {
    addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative paths with 'cppath'");
    return false;
  }
  if(dstPath.left(1) != "/" && jobDstPath.isEmpty()) {
    addStatus(FATAL, "Job destination path undefined. 'dstpath=PATH' has to be set when using relative paths with 'cppath'");
    return false;
  }

  QString srcDirString = srcPath;
  if(srcDirString.left(1) != "/") {
    srcDirString.prepend(jobSrcPath + "/");
  }
  QDir srcDir(srcDirString);

  QString dstDirString = dstPath;
  if(dstDirString.left(1) != "/") {
    dstDirString.prepend(jobDstPath + "/");
  }
  QDir dstDir(dstDirString);

  addStatus(STATUS, "Copying path '" + srcDir.absolutePath() + "' to '" + dstDir.absolutePath() + "'");

  if(!srcDir.exists()) {
    addStatus(FATAL, "Source path does not exist!");
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
      cpFile(itSrc.absoluteFilePath(), itDst.absoluteFilePath());
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
  
  addStatus(STATUS, "Running terminal command '" + fullCommand + "', please wait...");
  if(!pretend) {
    QProcess aptProc;
    aptProc.setProgram(program);
    aptProc.setArguments(args);
    aptProc.start();
    aptProc.waitForFinished(10 * 60 * 1000); // 10 minutes
    if(aptProc.exitStatus() != QProcess::NormalExit ||
       aptProc.exitCode() != 0) {
      addStatus(WARNING, "Terminal command failed!\n");
      addStatus(WARNING, "StdOut:\n" + aptProc.readAllStandardOutput());
      if(critical) {
        addStatus(FATAL, "StdError:\n" + aptProc.readAllStandardError());
        return false;
      } else {
        addStatus(WARNING, "StdError:\n" + aptProc.readAllStandardError());
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

bool JobRunner::rmPath(const QString &path, const bool &askPerFile)
{
  if(abortJob) {
    return false;
  }

  addStatus(STATUS, "Removing path '" + path + "' including subdirectories.");

  if(path.isEmpty()) {
    addStatus(FATAL, "Path not set, can't remove!");
    return false;
  }

  if(path.left(1) != "/") {
    addStatus(FATAL, "A non-relative path is required. Path not removed!");
    return false;
  }

  QDir rmDir(path);
  bool ask = askPerFile;
  {
    QDirIterator dirIt(rmDir.absolutePath(),
                       {"*"},
                       QDir::Files | QDir::NoDotAndDotDot,
                       QDirIterator::Subdirectories);
    while(dirIt.hasNext()) {
      dirIt.next();
      bool remove = true;
      if(ask) {
        MessageBox messageBox(QMessageBox::Question, "Delete?", "Delete file '" + dirIt.filePath() + "'?", QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll, this);
        messageBox.exec();
        if(messageBox.result() == QMessageBox::Yes ||
           messageBox.result() == QMessageBox::YesToAll) {
          if(messageBox.result() == QMessageBox::YesToAll) {
            ask = false;
          }
          remove = true;
        } else {
          remove = false;
        }
      }
      if(remove) {
        if(!pretend) {
          if(!rmFile(dirIt.filePath())) {
            return false;
          }
        }
      } else {
        addStatus(WARNING, "Skipping file '" + dirIt.filePath() + "'");
      }
    }
  }
  {
    QDirIterator dirIt(rmDir.absolutePath(),
                       {"*"},
                       QDir::Dirs | QDir::NoDotAndDotDot,
                       QDirIterator::Subdirectories);
    while(dirIt.hasNext()) {
      dirIt.next();
      bool remove = true;
      if(ask) {
        MessageBox messageBox(QMessageBox::Question, "Delete?", "Delete path '" + dirIt.filePath() + "'?", QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll, this);
        messageBox.exec();
        if(messageBox.result() == QMessageBox::Yes ||
           messageBox.result() == QMessageBox::YesToAll) {
          if(messageBox.result() == QMessageBox::YesToAll) {
            ask = false;
          }
          remove = true;
        } else {
          remove = false;
        }
      }
      if(remove) {
        addStatus(INFO, "Removing path '" + dirIt.filePath() + "'");
        if(!pretend) {
          if(!QDir::root().rmpath(dirIt.filePath())) {
            addStatus(FATAL, "Path could not be removed! Job cancelled.");
            return false;
          }
        }
      } else {
        addStatus(WARNING, "Skipping path '" + dirIt.filePath() + "'");
      }
    }
  }

  if(!pretend) {
    if(!QDir::root().rmpath(rmDir.absolutePath())) {
      addStatus(FATAL, "Base path could not be removed! Job cancelled.");
    }
  }

  addStatus(INFO, "Path removed!");
  return true;
}

bool JobRunner::rmFile(const QString &filePath)
{
  if(abortJob) {
    return false;
  }
  addStatus(INFO, "Removing file '" + filePath + "'");
  if(filePath.left(1) != "/") {
    addStatus(FATAL, "A non-relative file path is required. File not removed!");
    return false;
  }
  if(!QFile::exists(filePath)) {
    addStatus(FATAL, "File doesn't exist!");
    return false;
  }
  if(!QFile::remove(filePath)) {
    addStatus(FATAL, "File could not be removed. Maybe a permission problem?");
    return false;
  }
  addStatus(INFO, "File removed!");
  return true;
}

void JobRunner::setHardcodedVars()
{
  if(!QDir::homePath().isEmpty() && QDir::homePath().contains("/")) {
    // SEt %USER% so it can be used in the .job files
    vars["%USER%"] = QDir::homePath().split("/").last();
    // SEt %HOME% so it can be used in the .job files
    vars["%HOME%"] = QDir::homePath();
  }
  // SEt %WORKDIR% so it can be used in the .job files
  vars["%WORKDIR%"] = QDir::currentPath();
}

QString JobRunner::getCommandString(const Command &command)
{
  QString commandString = command.type + ":";
  for(const auto &parameter: command.parameters) {
    commandString.append(parameter + ";");
  }
  commandString = commandString.left(commandString.length() - 1);
  return commandString;
}

bool JobRunner::fileExclude(const QString &filename)
{
  addStatus(INFO, "Adding file exclude '" + filename + "'");
  fileExcludes.append(filename);
  return true;
}

bool JobRunner::pathExclude(const QString &path)
{
  addStatus(INFO, "Adding path exclude '" + path + "'");
  pathExcludes.append(path);
  return true;
}

bool JobRunner::setVar(const QString &key, const QString &value)
{
  QString variable = "%" + key + "%";
  addStatus(INFO, "Setting variable '" + variable + "' to value '" + value + "'");
  vars[variable] = value;
  varsReplace();
  return true;
}

bool JobRunner::loadVars(const QString &filename)
{
  addStatus(INFO, "Setting variables from file '" + filename + "'");
  QFile varsFile(filename);
  if(varsFile.open(QIODevice::ReadOnly)) {
    while(!varsFile.atEnd()) {
      QByteArray line = varsFile.readLine().trimmed();
      if(line.contains("=")) {
        setVar("%" + line.split('=').first() + "%", line.split('=').last());
      }
    }
    varsFile.close();
  } else {
    addStatus(FATAL, "Variable file '" + filename + "' could not be opened for reading!");
  }
  varsReplace();
  return true;
}

bool JobRunner::srcPath(const QString &path)
{
  QString tempPath = path;
  if(tempPath.right(1) == "/") {
    tempPath = tempPath.left(tempPath.length() - 1);
  }
  addStatus(INFO, "Setting source path to '" + tempPath + "'");
  if(!jobDstPath.isEmpty() &&
     tempPath.left(jobDstPath.length()) == jobDstPath &&
     jobDstPath.at(tempPath.length()) == "/") {
    addStatus(FATAL, "Source path is located beneath destination path. This is not allowed.");
    return false;
  } else {
    if(tempPath.left(1) != "/") {
      addStatus(FATAL, "Job source path '" + tempPath + "' is relative. This is not allowed.");
      return false;
    }
  }

  jobSrcPath = tempPath;
  vars["%SRCPATH%"] = jobSrcPath;
  varsReplace();
  return true;
}


bool JobRunner::dstPath(const QString &path)
{
  QString tempPath = path;
  if(tempPath.right(1) == "/") {
    tempPath = tempPath.left(tempPath.length() - 1);
  }
  addStatus(INFO, "Setting destination path to '" + tempPath + "'");
  if(!jobSrcPath.isEmpty() &&
     tempPath.left(jobSrcPath.length()) == jobSrcPath &&
     jobSrcPath.at(tempPath.length()) == "/") {
    addStatus(FATAL, "Destination path is located beneath source path. This is not allowed.");
    return false;
  } else {
    if(tempPath.left(1) != "/") {
      addStatus(FATAL, "Job destination path '" + tempPath + "' is relative. This is not allowed.");
      return false;
    }
  }

  jobDstPath = tempPath;
  vars["%DSTPATH%"] = jobDstPath;
  varsReplace();
  return true;
}

bool JobRunner::reboot(const QString &argument)
{
  if(argument == "force") {
    addStatus(INFO, "Forcing reboot now...");
    QProcess::execute("reboot", {});
  } else if(argument == "ask") {
    MessageBox messageBox(QMessageBox::Question, "Reboot?", "Do you wish to perform a system reboot?", QMessageBox::Yes | QMessageBox::No, this);
    messageBox.exec();
    if(messageBox.result() == QMessageBox::Yes) {
      addStatus(INFO, "User requested reboot, rebooting now...");
      QProcess::execute("reboot", {});
    } else {
      addStatus(WARNING, "User cancelled reboot.");
    }
  }
  return true;
}

bool JobRunner::shutdown(const QString &argument)
{
  if(argument == "force") {
    addStatus(INFO, "Forcing system shutdown now...");
    QProcess::execute("halt", {});
  } else if(argument == "ask") {
    MessageBox messageBox(QMessageBox::Question, "Shutdown?", "Do you wish to perform a system shutdown?", QMessageBox::Yes | QMessageBox::No, this);
    messageBox.exec();
    if(messageBox.result() == QMessageBox::Yes) {
      addStatus(INFO, "User requested shutdown, shutting down system now...");
      QProcess::execute("halt", {});
    } else {
      addStatus(WARNING, "User cancelled shutdown.");
    }
  }
  return true;
}
