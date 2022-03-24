/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jobrunner.h
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
#ifndef __VISUTEST_JOBRUNNER_H__
#define __VISUTEST_JOBRUNNER_H__

#include "mainsettings.h"

#include <QDialog>
#include <QEvent>
#include <QButtonGroup>
#include <QProgressBar>
#include <QListWidget>

constexpr int INFO = 0;
constexpr int STATUS = 1;
constexpr int WARNING = 2;
constexpr int FATAL = 3;

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

private:
  bool pretend = false;
  QList<Command> commands;
  QString jobSrcPath = "";
  QString jobDstPath = "";
  
  bool jobInProgress = false;
  bool abortJob = false;

  QListWidget *statusList = nullptr;
  QProgressBar *progressBar = nullptr;
  QList<QString> fileExcludes;
  QList<QString> pathExcludes;
  QButtonGroup *jobsButtons = nullptr;
  MainSettings &mainSettings;
  bool isExcluded(const QList<QString> &excludes, const QString &source);
  void runJob(const QString &filename);
  void addStatus(const int &status, const QString &text);
  bool cpFile(Command &command);
  bool cpPath(Command &command);
  bool runCommand(const QString &program, const QList<QString> &args, const bool &critical = false);
  QString varsReplace(QString string = "");

  QMap<QString, QString> vars;
  
};
#endif/*__VISUTEST_JOBRUNNER_H__*/
