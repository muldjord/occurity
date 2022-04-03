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
#include <QKeyEvent>
#include <QEventLoop>
#include <QNetworkInterface>
#include <QScrollBar>
#include <QStorageInfo>

JobRunner::JobRunner(MainSettings &mainSettings, QWidget *parent)
  : QDialog(parent), mainSettings(mainSettings)
{
  setWindowTitle("Occurity v" VERSION" jobrunner");
  setFixedSize(1200, 800);

  setStyleSheet("QLabel {font-weight:bold; margin-top: 5px;} QRadioButton {margin-left: 10px;}");
  
  jobsScroll = new QScrollArea;
  
  outputLabel = new QLabel("<h2>Output:</h2>");

  outputList = new QListWidget;
  outputList->setStyleSheet("QListWidget {background-color: black;}");
  outputList->setFocusPolicy(Qt::NoFocus);
  outputList->setWordWrap(true);
  
  progressBar = new QProgressBar;
  progressBar->setFormat("%p% completed");
  progressBar->setAlignment(Qt::AlignCenter);
  
  jobButtons = new QButtonGroup;
  connect(jobButtons, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &JobRunner::listFileContents);
  
  setHardcodedVars();

  QList<QVBoxLayout*> categoryLayouts;
  QList<QRadioButton *> jobButtonsList;
  QDir jobDir(getUsbPath().isEmpty()?mainSettings.jobsFolder:getUsbPath() + "/occurity/jobs", "*.job", QDir::Name, QDir::Files | QDir::NoDotAndDotDot);
  for(const auto &jobInfo: jobDir.entryInfoList()) {
    QFile jobFile(jobInfo.absoluteFilePath());
    if(jobFile.open(QIODevice::ReadOnly)) {
      QString jobTitle = jobInfo.fileName();
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

  QVBoxLayout *layout = new QVBoxLayout;

  if(!firstClicked) {
    layout->addWidget(new QLabel("<h2>No jobs found in jobs folder:</h2><h3>" + mainSettings.jobsFolder + "</h3>"), 2, Qt::AlignCenter);
  } else {
    jobsScroll->setFocusPolicy(Qt::NoFocus);
    jobsScroll->setMaximumHeight(250);
    jobsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget *jobsWidget = new QWidget;
    QVBoxLayout *jobsLayout = new QVBoxLayout;
    layout->addWidget(new QLabel("<h3>Choose job to run:</h3>"), 0, Qt::AlignCenter);
    for(auto *categoryLayout: categoryLayouts) {
      jobsLayout->addLayout(categoryLayout);
    }
    jobsWidget->setLayout(jobsLayout);
    jobsScroll->setWidget(jobsWidget);
    layout->addWidget(jobsScroll);
    layout->addWidget(outputLabel);
    layout->addWidget(outputList);
    layout->addWidget(progressBar);
  }
  setLayout(layout);
  
  installEventFilter(this);
}

void JobRunner::runJob(const QString &filename)
{
  abortJob = false;
  jobInProgress = true;
  progressBar->setFormat("%p% completed");
  progressBar->setValue(0);
  
  outputLabel->setText("<h3>Processing job file '" + filename + "':</h3>");
  outputList->clear();

  vars.clear();
  setHardcodedVars();

  jobSrcPath.clear();
  jobDstPath.clear();

  excludes.clear();

  commands.clear();

  pretend = false;

  addStatus(STATUS, "Job process begin!");

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
      command.type = line.left(line.indexOf(':')).trimmed();
      if(command.type == "title" ||
         command.type == "version" ||
         command.type == "category") {
        continue;
      }
      command.parameters = line.mid(line.indexOf(':') + 1).trimmed().split(';');
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
    if(command.type != "message" &&
       command.type != "exit") {
      addStatus(CODE, getCommandString(command));
    }

    // Check if %USBPATH% is used. Quit if it hasn't been set
    for(const auto &parameter: command.parameters) {
      if(parameter.contains("%USBPATH%") && !vars.contains("%USBPATH%")) {
        addStatus(FATAL, "%USBPATH% used but not detected! USB device must be named 'USBPEN' and has to be inserted prior to opening job runner. Job cancelled!");
        break;
      }
    }

    if(abortJob) {
      break;
    }

    if(command.type == "aptinstall") {
      if(command.parameters.length() >= 1) {
        if(hasInternet(getCommandString(command))) {
          if(runCommand("sudo", { "apt-get", "--help" }, 1)) {
            runCommand("sudo", { "apt-get", "-y", "update" });
            runCommand("sudo", (QList<QString> { "apt-get", "-y", "install" }) + command.parameters);
          } else {
            addStatus(FATAL, "Current user is probably not set up to use 'apt-get' in '/etc/sudoers/sudoers.d/'. Check 'README.md' on how to correct this.");
          }
        }
      }

    } else if(command.type == "aptremove") {
      if(command.parameters.length() == 1) {
        if(hasInternet(getCommandString(command))) {
          if(runCommand("sudo", { "apt-get", "--help" }, 1)) {
            runCommand("sudo", { "apt-get", "-y", "update" });
            runCommand("sudo", (QList<QString> { "apt-get", "-y", "remove" }) + command.parameters);
          } else {
            addStatus(FATAL, "Current user is probably not set up to use 'apt-get' in '/etc/sudoers/sudoers.d/'. Check 'README.md' on how to correct this.");
          }
        }
      }

    } else if(command.type == "addexclude") {
      if(command.parameters.length() == 1) {
        addExclude(command.parameters.at(0));
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

    } else if(command.type == "pretend") {
      if(command.parameters.length() == 1) {
        if(command.parameters.at(0) == "true") {
          addStatus(STATUS, "Setting pretend! No path or file changes will occur while processing this job.");
          pretend = true;
        }
      }

    } else if(command.type == "setexec") {
      if(command.parameters.length() == 1) {
        runCommand("chmod", { "+x", command.parameters.at(0) });
      }

    } else if(command.type == "cpfile") {
      if(command.parameters.length() == 1 || command.parameters.length() == 2) {
        cpFile(command.parameters.first(), command.parameters.last());
      }

    } else if(command.type == "mvfile") {
      if(command.parameters.length() == 1) {
        mvFile(command.parameters.at(0));
      } else if(command.parameters.length() == 2) {
        mvFile(command.parameters.at(0), command.parameters.at(1));
      }

    } else if(command.type == "rmfile") {
      if(command.parameters.length() == 1) {
        rmFile(command.parameters.at(0));
      }

    } else if(command.type == "cppath") {
      if(command.parameters.length() == 1 || command.parameters.length() == 2) {
        cpPath(command.parameters.at(0), command.parameters.last());
      }

    } else if(command.type == "mvpath") {
      if(command.parameters.length() == 1) {
        mvPath(command.parameters.at(0));
      } else if(command.parameters.length() == 2) {
        mvPath(command.parameters.at(0), command.parameters.at(1));
      }

    } else if(command.type == "rmpath") {
      if((command.parameters.length() == 1 || command.parameters.length() == 2)) {
        bool askPerPath = (command.parameters.length() == 2 && command.parameters.at(1) == "ask"?true:false);
        rmPath(command.parameters.at(0), askPerPath);
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

    } else if(command.type == "exit") {
      if(command.parameters.length() == 1) {
        addStatus(STATUS, "Exit: '" + command.parameters.at(0) + "'");
        break;
      }

    }
    progressBar->setValue(progressBar->value() + 1);
  }
  if(abortJob) {
    progressBar->setFormat("Job process failed at %p% :(");
  } else {
    progressBar->setFormat("Job process completed! :)");
    progressBar->setValue(progressBar->maximum());
  }
  jobInProgress = false;
}

bool JobRunner::isExcluded(const QString &src)
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
  itemFont.setWeight(QFont::Bold);
  int delay = 10;
  item->setFont(itemFont);
  if(status == INFO) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::green));
    item->setText(QString("    ") + (pretend?"pretend: ":"") + text);
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
  } else if(status == CODE) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::gray));
    item->setText(QString("  ") + (pretend?"pretend: ":"") + text);
  } else if(status == INIT) {
    qInfo("%s", text.toUtf8().data());
    item->setForeground(QBrush(Qt::cyan));
    item->setText(QString("  ") + (pretend?"pretend: ":"") + text);
  }
  outputList->addItem(item);
  outputList->scrollToBottom();
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
  
  addStatus(INIT, "Copying file '" + srcInfo.absoluteFilePath() + "' to '" + dstInfo.absoluteFilePath() + "'");

  if(isExcluded(srcInfo.absoluteFilePath())) {
    addStatus(WARNING, "Source file marked for exclusion, continuing without copying!");
    return true;
  }

  if(!QFile::exists(srcInfo.absoluteFilePath())) {
    addStatus(FATAL, "Source file not found!");
    return false;
  }

  if(srcInfo.absoluteFilePath() == dstInfo.absoluteFilePath()) {
    addStatus(WARNING, "Source and destination are the same! Ignoring.");
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

  QString srcDirString = srcPath;
  if(srcDirString.left(1) != "/") {
    if(jobSrcPath.isEmpty()) {
      addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative paths with 'cppath'");
      return false;
    }
    srcDirString.prepend(jobSrcPath + "/");
  }
  QString dstDirString = dstPath;
  if(dstDirString.left(1) != "/") {
    if(jobDstPath.isEmpty()) {
      addStatus(FATAL, "Job destination path undefined. 'dstpath=PATH' has to be set when using relative paths with 'cppath'");
      return false;
    }
    dstDirString.prepend(jobDstPath + "/");
  }

  QDir srcDir(srcDirString, "*", QDir::Name, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
  QDir dstDir(dstDirString);

  addStatus(INIT, "Copying path '" + srcDir.absolutePath() + "' to '" + dstDir.absolutePath() + "'");

  if(isExcluded(srcDir.absolutePath())) {
    addStatus(WARNING, "Source path marked for exclusion, continuing without copying!");
    return true;
  }
  
  if(!pretend && !QDir::root().mkpath(dstDir.absolutePath())) {
    addStatus(FATAL, "Path '" + dstDir.absolutePath() + "' could not be copied!");
    return false;
  }

  for(const auto &dirInfo: srcDir.entryInfoList()) {
    if(dirInfo.isDir()) {
      if(!cpPath(dirInfo.absoluteFilePath(), dstDir.absolutePath() + "/" + dirInfo.absoluteFilePath().mid(srcDirString.length() + 1))) {
        return false;
      }
    } else if(dirInfo.isFile()) {
      if(!cpFile(dirInfo.absoluteFilePath(), dstDir.absolutePath() + "/" + dirInfo.absoluteFilePath().mid(srcDirString.length() + 1))) {
        return false;
      }
    }
  }

  addStatus(INFO, "Path '" + srcDir.absolutePath() + "' copied successfully!");
  return true;
}

bool JobRunner::runCommand(const QString &program, const QList<QString> &args, const int &maxWaitSecs, const bool &critical)
{
  QString fullCommand = program + " ";
  for(const auto &arg: args) {
    fullCommand.append(arg + " ");
  }
  fullCommand = fullCommand.trimmed();
  
  addStatus(INIT, "Running terminal command '" + fullCommand + "', please wait...");
  if(!pretend) {
    QProcess terminal;
    terminal.setProgram(program);
    terminal.setArguments(args);
    terminal.start();
    if(critical && !terminal.waitForFinished(maxWaitSecs * 1000)) {
      addStatus(FATAL, "Terminal command timed out!");
      return false;
    }
    if(terminal.exitStatus() != QProcess::NormalExit ||
       terminal.exitCode() != 0) {
      addStatus(INFO, "StdOut:\n" + terminal.readAllStandardOutput());
      if(critical) {
        addStatus(WARNING, "StdError:\n" + terminal.readAllStandardError());
        addStatus(FATAL, "Terminal command failed!");
        return false;
      } else {
        addStatus(WARNING, "StdError:\n" + terminal.readAllStandardError());
        addStatus(WARNING, "Terminal command failed, but is not critical!");
      }
      return false;
    }
    
    addStatus(INFO, "Terminal command completed successfully with the following output:");
    addStatus(INFO, terminal.readAllStandardOutput().data());
    addStatus(INFO, terminal.readAllStandardError().data());
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

bool JobRunner::rmPath(const QString &path, bool &askPerPath)
{
  if(abortJob) {
    return false;
  }

  addStatus(INIT, "Entering path '" + path + "'");

  if(path.left(1) != "/") {
    addStatus(FATAL, "A non-relative path is required. No path removed!");
    return false;
  }

  QDir srcDir(path, "*", QDir::Name, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

  if(isExcluded(srcDir.absolutePath())) {
    addStatus(WARNING, "Path marked for exclusion, continuing without removing!");
    return true;
  }
  if(!srcDir.exists()) {
    addStatus(WARNING, "Path doesn't exist! Ignoring.");
    return true;
  }
  
  if(askPerPath) {
    MessageBox messageBox(QMessageBox::Question, "Delete?", "Do you want to delete path '" + path + "'?", QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No, this);
    messageBox.exec();
    if(messageBox.result() == QMessageBox::Yes ||
       messageBox.result() == QMessageBox::YesToAll) {
      if(messageBox.result() == QMessageBox::YesToAll) {
        askPerPath = false;
      }
    } else {
      addStatus(WARNING, "Path not removed!");
      return true;
    }
  }
  
  for(const auto &dirInfo: srcDir.entryInfoList()) {
    if(dirInfo.isDir()) {
      if(!rmPath(dirInfo.absoluteFilePath(), askPerPath)) {
        return false;
      }
    } else if(dirInfo.isFile()) {
      if(!rmFile(dirInfo.absoluteFilePath())) {
        return false;
      }
    }
  }

  addStatus(INIT, "Removing path '" + path + "'");
  if(!pretend) {
    srcDir.refresh();
    if(srcDir.count()) {
      for(const auto &tempInfo: srcDir.entryInfoList()) {
        printf("FOUND: %s\n", tempInfo.absoluteFilePath().toStdString().c_str());
      }
      addStatus(WARNING, "Path '" + srcDir.absolutePath() + "' is not empty, can't remove!");
      return false;
    } else if(!QDir::root().rmdir(srcDir.absolutePath())) {
      addStatus(FATAL, "Path '" + srcDir.absolutePath() + "' could not be removed! Perhaps a permission problem?");
      return false;
    }
  }

  addStatus(INFO, "Path '" + srcDir.absolutePath() + "' removed successfully!");
  return true;
}

bool JobRunner::rmFile(const QString &filePath)
{
  if(abortJob) {
    return false;
  }

  addStatus(INIT, "Removing file '" + filePath + "'");

  if(filePath.left(1) != "/") {
    addStatus(FATAL, "A non-relative file path is required. No file removed!");
    return false;
  }

  if(isExcluded(filePath)) {
    addStatus(WARNING, "File marked for exclusion, file not removed!");
    return true;
  }

  if(!QFile::exists(filePath)) {
    addStatus(FATAL, "File doesn't exist!");
    return false;
  }

  if(!QFile::remove(filePath)) {
    addStatus(FATAL, "File could not be removed!");
    return false;
  }

  addStatus(INFO, "File removed successfully!");
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

  QString usbPath = getUsbPath();
  if(!usbPath.isEmpty()) {
    // SEt %WORKDIR% so it can be used in the .job files
    vars["%USBPATH%"] = usbPath;
  }
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

bool JobRunner::addExclude(const QString &exclude)
{
  addStatus(INIT, "Adding exclude '" + exclude + "'");
  excludes.append(exclude);
  return true;
}

bool JobRunner::setVar(QString variable, const QString &value)
{
  if(variable.contains("%")) {
    addStatus(WARNING, "Variable name contains '%'. This is discouraged and can cause problems!");
  }
  variable.prepend("%");
  variable.append("%");
  addStatus(INIT, "Setting variable '" + variable + "' to value '" + value + "'");
  vars[variable] = value;
  varsReplace();
  addStatus(INFO, "Variable set succesfully!");
  return true;
}

bool JobRunner::loadVars(QString filename)
{
  if(filename.left(1) != "/") {
    if(jobSrcPath.isEmpty()) {
      addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative paths with 'loadvars'");
      return false;
    }
    filename.prepend(jobSrcPath + "/");
  }

  addStatus(INIT, "Setting variables from file '" + filename + "'");
  QFile varsFile(filename);
  if(varsFile.open(QIODevice::ReadOnly)) {
    while(!varsFile.atEnd()) {
      QByteArray line = varsFile.readLine().trimmed();
      if(line.contains("=")) {
        setVar(line.split('=').first(), line.split('=').last());
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
  addStatus(INIT, "Setting source path to '" + tempPath + "'");
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
  addStatus(INIT, "Setting destination path to '" + tempPath + "'");
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
  addStatus(INIT, "System reboot requested...");
  if(argument == "force") {
    addStatus(INFO, "Forcing reboot now!");
    QProcess::execute("reboot", {});
  } else if(argument == "ask") {
    MessageBox messageBox(QMessageBox::Question, "Reboot?", "Do you wish to perform a system reboot?", QMessageBox::Yes | QMessageBox::No, this);
    messageBox.exec();
    if(messageBox.result() == QMessageBox::Yes) {
      addStatus(INFO, "Accepted by user, rebooting now...");
      QProcess::execute("reboot", {});
    } else {
      addStatus(WARNING, "Cancelled by user!");
    }
  }
  return true;
}

bool JobRunner::shutdown(const QString &argument)
{
  addStatus(INIT, "System shutdown requested...");
  if(argument == "force") {
    addStatus(INFO, "Forcing shutdown now!");
    QProcess::execute("halt", {});
  } else if(argument == "ask") {
    MessageBox messageBox(QMessageBox::Question, "Shutdown?", "Do you wish to perform a system shutdown?", QMessageBox::Yes | QMessageBox::No, this);
    messageBox.exec();
    if(messageBox.result() == QMessageBox::Yes) {
      addStatus(INFO, "Accepted by user, shutting system down now...");
      QProcess::execute("halt", {});
    } else {
      addStatus(WARNING, "Cancelled by user!");
    }
  }
  return true;
}

bool JobRunner::hasInternet(const QString &command)
{
  for(const auto &addr: QNetworkInterface::allAddresses()) {
    if(addr.isGlobal()) {
      return true;
    }
  }
  MessageBox messageBox(QMessageBox::Question, "No internet", "The command '" + command + "' requires an internet connection.\n\nIt seems you are not connected to the internet! Is this command critical for this job procedure to proceed as expected?", QMessageBox::Yes | QMessageBox::No, this);
  messageBox.exec();
  if(messageBox.result() == QMessageBox::Yes) {
    addStatus(FATAL, "Job cancelled due to missing internet connection!");
  } else {
    addStatus(WARNING, "No internet connection detected, continuing anyway!");
  }
  return false;
}

bool JobRunner::mvFile(QString srcFile, QString dstFile)
{
  if(abortJob) {
    return false;
  }

  if(srcFile.left(1) != "/" && jobSrcPath.isEmpty()) {
    addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative file paths with 'mvfile'");
    return false;
  } else {
    srcFile.prepend(jobSrcPath + "/");
  }

  if(dstFile.isEmpty()) {
    dstFile = srcFile;
  }

  if(dstFile.left(1) != "/" && jobDstPath.isEmpty()) {
    addStatus(FATAL, "Job destination path undefined. 'dstpath=PATH' has to be set when using relative file paths with 'mvfile'");
    return false;
  } else {
    dstFile.prepend(jobDstPath + "/");
  }

  if(!serialize(dstFile)) {
    addStatus(FATAL, "4-digit backup serial number could not be generated, job cancelled!");
    return false;
  }

  QFileInfo srcInfo(srcFile);
  QFileInfo dstInfo(dstFile);

  addStatus(INIT, "Moving file '" + srcInfo.absoluteFilePath() + "' to '" + dstInfo.absoluteFilePath() + "'");

  if(!QFile::exists(srcInfo.absoluteFilePath())) {
    addStatus(WARNING, "Source file not found! Ignoring.");
    return true;
  }

  if(srcInfo.absoluteFilePath() == dstInfo.absoluteFilePath()) {
    addStatus(WARNING, "Source and destination are the same! Ignoring.");
    return true;
  }

  if(QFile::exists(dstInfo.absoluteFilePath())) {
    addStatus(FATAL, "Destination file already exists!");
    return false;
  }
  
  if(!pretend && !QFile::rename(srcInfo.absoluteFilePath(), dstInfo.absoluteFilePath())) {
    addStatus(FATAL, "File move failed!");
    return false;
  }
  
  addStatus(INFO, "File move successful!");
  return true;
}

bool JobRunner::mvPath(QString srcPath, QString dstPath)
{
  if(abortJob) {
    return false;
  }

  if(srcPath.left(1) != "/" && jobSrcPath.isEmpty()) {
    addStatus(FATAL, "Job source path undefined. 'srcpath=PATH' has to be set when using relative paths with 'mvpath'");
    return false;
  } else {
    srcPath.prepend(jobSrcPath + "/");
  }

  if(dstPath.isEmpty()) {
    dstPath = srcPath;
  }

  if(dstPath.left(1) != "/" && jobDstPath.isEmpty()) {
    addStatus(FATAL, "Job destination path undefined. 'dstpath=PATH' has to be set when using relative paths with 'mvpath'");
    return false;
  } else {
    dstPath.prepend(jobDstPath + "/");
  }

  if(!serialize(dstPath)) {
    addStatus(FATAL, "4-digit backup serial number could not be generated, job cancelled!");
    return false;
  }

  QFileInfo srcInfo(srcPath);
  QFileInfo dstInfo(dstPath);

  addStatus(INIT, "Moving path '" + srcInfo.absoluteFilePath() + "' to '" + dstInfo.absoluteFilePath() + "'");

  if(!QFile::exists(srcInfo.absoluteFilePath())) {
    addStatus(WARNING, "Source path not found! Ignoring.");
    return true;
  }

  if(srcInfo.absoluteFilePath() == dstInfo.absoluteFilePath()) {
    addStatus(WARNING, "Source and destination are the same! Ignoring.");
    return true;
  }

  if(QFile::exists(dstInfo.absoluteFilePath())) {
    addStatus(FATAL, "Destination path already exists!");
    return false;
  }
  
  if(!pretend && !QFile::rename(srcInfo.absoluteFilePath(), dstInfo.absoluteFilePath())) {
    addStatus(FATAL, "Path move failed!");
    return false;
  }
  
  addStatus(INFO, "Path move successful!");
  return true;
}

void JobRunner::listFileContents(QAbstractButton *button)
{
  QRadioButton *selectedButton = reinterpret_cast<QRadioButton*>(button);
  if(prevButton != nullptr) {
    int delta = selectedButton->pos().y() - prevButton->pos().y();
    jobsScroll->verticalScrollBar()->setValue(jobsScroll->verticalScrollBar()->value() + delta);     }
  QString filename = button->objectName();
  progressBar->setFormat("No job in progress");
  progressBar->setValue(0);
  outputList->clear();
  QFile jobFile(filename);
  if(jobFile.exists() && jobFile.open(QIODevice::ReadOnly)) {
    outputLabel->setText("<h3>Showing contents of job file '" + filename + "':</h3>");
    while(!jobFile.atEnd()) {
      QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8(jobFile.readLine().trimmed()));
      QFont itemFont("monospace");
      item->setFont(itemFont);
      item->setForeground(QBrush(Qt::white));
      outputList->addItem(item);
    }
    jobFile.close();
  } else {
    outputLabel->setText("<h3>Couldn't open job file '" + filename + "'!</h3>");
  }
  prevButton = selectedButton;
}

QString JobRunner::getUsbPath()
{
  for(const auto &volume: QStorageInfo::mountedVolumes()) {
    if(volume.fileSystemType().toLower().contains("fat") &&
       (volume.displayName() == "USBPEN" || volume.name() == "USBPEN")) {
      return volume.rootPath();
    }
  }
  return "";
}

bool JobRunner::serialize(QString &dst)
{
  if(!QFile::exists(dst)) {
    return true;
  }
  addStatus(WARNING, "Destination file or path already exists, adding serial number!");
  dst.append("0000");
  int serial = 0;
  do {
    dst = dst.left(dst.length() - 4);
    if(serial > 9999) {
      return false;
    }
    QString serialString = QString::number(serial);
    serial++;
    while(serialString.length() < 4) {
      serialString.prepend("0");
    }
    dst += serialString;
  } while(QFile::exists(dst));
  return true;
}
