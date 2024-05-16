/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.h
 *
 *  Tue Mar 13 17:00:00 UTC+1 2018
 *  Copyright 2018 Lars Bisballe
 *  larsbjensen@gmail.com
 ****************************************************************************/

/*
 *  This file is part of Occurity.
 *
 *  Occurity is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
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
#ifndef __OCCURITY_MAINWINDOW_H__
#define __OCCURITY_MAINWINDOW_H__

#include "abstractchart.h"
#include "svgchart.h"
#include "optotypechart.h"
#include "mainsettings.h"

#include <QGraphicsView>
#include <QSettings>
#include <QKeyEvent>
#include <QTimer>

class MainWindow : public QGraphicsView
{
  Q_OBJECT

public:
  MainWindow(QSettings &config);
  ~MainWindow();

protected:
  bool eventFilter(QObject *, QEvent *event);

signals:
  void configUpdated();

private slots:
  void init();
  void monitorSleep();
  void monitorOn();

private:
  QSettings &config;
  MainSettings mainSettings;
  VideoPlayer *videoPlayer = nullptr;
  void loadFonts(QString dirStr);
  bool loadCharts(QString chartsXml);
  void spawnPreferences();
  void spawnJobRunner();
  void updateFromConfig();
  QList<AbstractChart*> charts;

  // Sleep timer that puts monitor to sleep after X minutes of inactivity
  QTimer sleepTimer;

  // Timer that resets all charts after being idle for configured time
  QTimer resetTimer;

  // The following bool is needed to make a 'q' button press work for making the monitor wake up again
  bool monitorIsOn = true;

  // Timer used to delay sleep activation to avoid the monitor turning back on due to remote control double-presses
  QTimer delayedSleepTimer;

  // Opposite of the above. If 'q' is pressed while monitor is off the monitorIsOn bool must be set to 'true' after a delay to avoid a double-press to keep turning it off while it already is off.
  QTimer delayedMonitorOnTimer;
};
#endif/*__OCCURITY_MAINWINDOW_H__*/
