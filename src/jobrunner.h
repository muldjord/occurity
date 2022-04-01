/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jobrunner.h
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
#ifndef __OCCURITY_JOBRUNNER_H__
#define __OCCURITY_JOBRUNNER_H__

#include "mainsettings.h"

#include <QDialog>
#include <QEvent>
#include <QButtonGroup>
#include <QProgressBar>
#include <QListWidget>
#include <QLabel>
#include <QScrollArea>
#include <QRadioButton>

constexpr int INFO = 0;
constexpr int STATUS = 1;
constexpr int WARNING = 2;
constexpr int FATAL = 3;
constexpr int CODE = 4;
constexpr int INIT = 5;

struct Command {
  QString type = "";
  QList<QString> parameters;
};

class JobRunner : public QDialog
{
  Q_OBJECT

public:
  JobRunner(MainSettings &mainSettings, QWidget *parent);

protected:
  bool eventFilter(QObject *, QEvent *event) override;

private slots:
  void listFileContents(QAbstractButton *button);

private:
  bool pretend = false;
  QList<Command> commands;
  QString jobSrcPath = "";
  QString jobDstPath = "";
  
  bool jobInProgress = false;
  bool abortJob = false;

  QRadioButton *prevButton = nullptr;

  QScrollArea *jobsScroll = nullptr;
  QLabel *outputLabel = nullptr;
  QListWidget *outputList = nullptr;
  QProgressBar *progressBar = nullptr;
  QList<QString> excludes;
  QButtonGroup *jobButtons = nullptr;
  MainSettings &mainSettings;

  void setHardcodedVars();
  bool addExclude(const QString &exclude);
  bool isExcluded(const QString &src);
  bool setVar(QString variable, const QString &value);
  bool loadVars(QString filename);
  void runJob(const QString &filename);
  void addStatus(const int &status, const QString &text);

  bool srcPath(const QString &path);
  bool dstPath(const QString &path);

  bool cpFile(const QString &srcFile, const QString &dstFile);
  bool mvFile(QString srcFile, QString dstFile = "");
  bool rmFile(const QString &filePath);

  bool cpPath(const QString &srcPath, const QString &dstPath);
  bool mvPath(QString srcPath, QString dstPath = "");
  bool rmPath(const QString &path, bool &askPerPath);

  bool hasInternet(const QString &command);
  bool runCommand(const QString &program, const QList<QString> &args, const int &maxWaitSecs = 600, const bool &critical = true);
  bool reboot(const QString &argument);
  bool shutdown(const QString &argument);
  QString varsReplace(QString string = "");
  QString getCommandString(const Command &command);

  QMap<QString, QString> vars;
  
};
#endif/*__OCCURITY_JOBRUNNER_H__*/
