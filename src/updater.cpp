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

Updater::Updater(MainSettings &mainSettings, QWidget *parent)
  : QWizard(parent), mainSettings(mainSettings)
{
  setWindowTitle(tr("VisuTest updater"));
  setFixedSize(500, 200);

  // Update type
  applyUpdateButton = new QRadioButton(tr("Apply update from update folder"));
  applyUpdateButton->click();
  //QRadioButton *createUpdateButton = new QRadioButton(tr("Create update from this version"));
  /*
  
  QVBoxLayout *typeLayout = new QVBoxLayout;
  typeLayout->addWidget(applyUpdateButton);

  QWizardPage *typePage = new QWizardPage;
  typePage->setLayout(typeLayout);
  */
  // Apply update page
  QVBoxLayout *updateLayout = new QVBoxLayout;
  updateButtons = new QButtonGroup;
  QList<QString> filters { "*.upd" };
  QDirIterator dirIt(mainSettings.updateBaseFolder,
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
          } else if(line.split(":").first() == "version") {
            updVersion = line.split(":").last();
          }
        }
      }
      if(!updTitle.isEmpty()) {
        QRadioButton *updateTypeButton = new QRadioButton(updVersion + ": " + updTitle);
        if(!foundUpdate) {
          updateTypeButton->click();
        }
        updateTypeButton->setObjectName(dirIt.fileInfo().absoluteFilePath());
        updateButtons->addButton(updateTypeButton);
        updateLayout->addWidget(updateTypeButton);
        foundUpdate = true;
      }
      updFile.close();
    }
  }

  QWizardPage *updatePage = new QWizardPage;
  updatePage->setLayout(updateLayout);

  //addPage(typePage);
  addPage(updatePage);
}

void Updater::applyUpdate(const QString &filename)
{
  qInfo("Applying update from file: '%s'\n", filename.toStdString().c_str());
  QFileInfo updInfo(filename);
  mainSettings.updateBaseFolder = updInfo.absolutePath();
  if(!updInfo.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("The update file") + " '" + filename + "' " + tr("does not exist. Can't update, aborting!"));
    return;
  }
  QList<Command> commands;
  QList<QString> fileExcludes;
  QList<QString> pathExcludes;
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

  for(const auto &command: commands) {
    if(command.type == "aptinstall" && command.parameters.length()) {
      qInfo("Running apt install...\n");
      QProcess aptProc;
      aptProc.setProgram("sudo");
      aptProc.setArguments(QList<QString> {"apt-get", "update"});
      aptProc.start();
      aptProc.waitForFinished(30 * 60 * 1000); // 30 minutes
      QList<QString> parameters {"apt-get", "install", "--force-yes"};
      parameters.append(command.parameters);
      aptProc.setArguments(parameters);
      aptProc.start();
      aptProc.waitForFinished(30 * 60 * 1000); // 30 minutes
      if(aptProc.exitStatus() == QProcess::NormalExit) {
        if(aptProc.exitCode() != 0) {
          qCritical("Apt install process failed:\n%s\n", aptProc.readAllStandardError().data());
          QMessageBox::critical(this, tr("Warning"), tr("The apt install command failed. See 'debug.log' for more information. Continuing anyway!"));
        } else {
          qInfo("%s\n", aptProc.readAllStandardError().data());
          qInfo("%s\n", aptProc.readAllStandardOutput().data());
        }
      } else {
        qCritical("Apt QProcess did not complete with 'QProcess::NormalExit' status!\n");
        QMessageBox::critical(this, tr("Warning"), tr("Apt QProcess did not complete with 'QProcess::NormalExit' status. See 'debug.log' for more information. Continuing anyway!"));
      }
    } else if(command.type == "aptremove" && command.parameters.length()) {
      qInfo("Running apt remove...\n");
      QProcess aptProc;
      aptProc.setProgram("sudo");
      aptProc.setArguments(QList<QString> {"apt-get", "update"});
      aptProc.start();
      aptProc.waitForFinished(30 * 60 * 1000); // 30 minutes
      QList<QString> parameters {"apt-get", "remove", "--force-yes"};
      parameters.append(command.parameters);
      aptProc.setArguments(parameters);
      aptProc.start();
      aptProc.waitForFinished(30 * 60 * 1000); // 30 minutes
      if(aptProc.exitStatus() == QProcess::NormalExit) {
        if(aptProc.exitCode() != 0) {
          qCritical("Apt remove process failed:\n%s\n", aptProc.readAllStandardError().data());
          QMessageBox::critical(this, tr("Warning"), tr("The apt remove command failed. See 'debug.log' for more information. Continuing anyway!"));
        } else {
          qInfo("%s\n", aptProc.readAllStandardError().data());
          qInfo("%s\n", aptProc.readAllStandardOutput().data());
        }
      } else {
        qCritical("Apt QProcess did not complete with 'QProcess::NormalExit' status!\n");
        QMessageBox::critical(this, tr("Warning"), tr("Apt QProcess did not complete with 'QProcess::NormalExit' status. See 'debug.log' for more information. Continuing anyway!"));
      }
    } else if(command.type == "fileexclude" && command.parameters.length() == 1) {
      qInfo("Adding to file excludes: '%s'\n", command.parameters.first().toStdString().c_str());
      fileExcludes.append(command.parameters.first());
    } else if(command.type == "pathexclude" && command.parameters.length() == 1) {
      qInfo("Adding to path excludes: '%s'\n", command.parameters.first().toStdString().c_str());
      pathExcludes.append(command.parameters.first());
    } else if(command.type == "cpfile" &&
              (command.parameters.length() == 1 || command.parameters.length() == 2)) {
      QFileInfo src(mainSettings.updateBaseFolder + "/" + command.parameters.first());
      QFileInfo dst(QDir::currentPath() + "/" + command.parameters.last());
      qInfo("Preparing to copy file:\n  '%s'\n->'%s'\n",
             src.absoluteFilePath().toStdString().c_str(),
             dst.absoluteFilePath().toStdString().c_str());
      if(isExcluded(fileExcludes, src.absoluteFilePath())) {
        continue;
      }
      if(!QFile::exists(src.absoluteFilePath())) {
        return;
      }
      QDir::current().mkpath(dst.absolutePath());
      if(QFile::exists(dst.absoluteFilePath())) {
        if(!QFile::remove(dst.absoluteFilePath())) {
          qCritical("  Destination file could not be removed before copy. Update cancelled!\n");
          QMessageBox::critical(this, tr("Error"), tr("Destination file could not be removed before copy. Update cancelled!"));
          return;
        }
      }
      if(QFile::copy(src.absoluteFilePath(), dst.absoluteFilePath())) {
        qInfo("  File copied succesfully!\n");
      } else {
        qCritical("  File copy failed. Update cancelled!\n");
        QMessageBox::critical(this, tr("Error"), tr("File copy failed. Update cancelled!"));
        return;
      }
    } else if(command.type == "cppath" &&
              command.parameters.length() == 1) {
      QDir src(mainSettings.updateBaseFolder + "/" + command.parameters.first());
      QDir dst(QDir::currentPath() + "/" + command.parameters.first());
      qInfo("Preparing to copy path:\n  '%s'\n->'%s'\n",
             src.absolutePath().toStdString().c_str(),
             dst.absolutePath().toStdString().c_str());
      if(!src.exists()) {
        qWarning("  Path '%s' does not exist. Update cancelled!\n", src.absolutePath().toStdString().c_str());
        QMessageBox::critical(this, tr("Error"), tr("Path '") + src.absolutePath() + tr("' does not exist. Update cancelled!"));
        return;
      }
      QDirIterator dirIt(src.absolutePath(),
                         {"*"},
                         QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                         QDirIterator::Subdirectories);
      while(dirIt.hasNext()) {
        dirIt.next();
        QFileInfo itSrc = dirIt.fileInfo();
        QFileInfo itDst(QDir::currentPath() + "/" + itSrc.absoluteFilePath().mid(mainSettings.updateBaseFolder.length()));
        if(isExcluded(fileExcludes, itSrc.absoluteFilePath())) {
          continue;
        }
        if(isExcluded(pathExcludes, itSrc.absoluteFilePath())) {
          continue;
        }
        if(!QFileInfo::exists(itDst.absolutePath())) {
          if(QDir::current().mkpath(itDst.absolutePath())) {
            qInfo("  Path '%s' created succesfully!\n", itDst.absolutePath().toStdString().c_str());
          } else {
            qCritical("  Path '%s' could not be created. Update cancelled!\n", itDst.absolutePath().toStdString().c_str());
            QMessageBox::critical(this, tr("Error"), tr("Path could not be created. Update cancelled!"));
            return;
          }
        }
        if(itSrc.isFile()) {
          if(QFile::exists(itDst.absoluteFilePath())) { 
            if(!QFile::remove(itDst.absoluteFilePath())) {
              qCritical("  Destination file '%s' could not be removed before copy. Update cancelled!\n", itDst.absoluteFilePath().toStdString().c_str());
              QMessageBox::critical(this, tr("Error"), tr("Destination file could not be removed before copy. Update cancelled!"));
              return;
            }
          }
          if(QFile::copy(itSrc.absoluteFilePath(), itDst.absoluteFilePath())) {
            qInfo("  File '%s' copied succesfully!\n", itSrc.absoluteFilePath().toStdString().c_str());
          } else {
            qCritical("  File '%s' could not be copied. Update cancelled!\n", itSrc.absoluteFilePath().toStdString().c_str());
            QMessageBox::critical(this, tr("Error"), tr("File copy failed. Update cancelled!"));
            return;
          }
        }
      }
    }
  }
  qInfo("Update completed!\n");
  QMessageBox::information(this, tr("Update completed"), tr("Update completed successfully!\n\nPress OK to reboot the system."));
  QProcess::execute("reboot", {});
}

bool Updater::isExcluded(const QList<QString> &excludes, const QString &src)
{
  QFileInfo srcInfo(src);
  for(const auto &exclude: excludes) {
    QFileInfo excludeInfo(mainSettings.updateBaseFolder + "/" + exclude);
    //qInfo("Source : '%s'\n", srcInfo.absoluteFilePath().toStdString().c_str());
    //qInfo("Exclude: '%s'\n", excludeInfo.absoluteFilePath().toStdString().c_str());
    if(srcInfo.absoluteFilePath().left(excludeInfo.absoluteFilePath().length()) == excludeInfo.absoluteFilePath()) {
      qInfo("  Excluded: '%s'\n", src.toStdString().c_str());
      return true;
    }
  }
  return false;
}

void Updater::accept()
{
  if(applyUpdateButton->isChecked() &&
     !updateButtons->buttons().isEmpty()) {
    for(const auto *button: updateButtons->buttons()) {
      if(button->isChecked()) {
        applyUpdate(button->objectName());
        break;
      }
    }
  }
  QDialog::accept();
}
