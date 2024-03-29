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
  void hibernate();

private:
  void flipHibernate(bool forceHibernate = false);

  QSettings &config;
  MainSettings mainSettings;
  VideoPlayer *videoPlayer = nullptr;
  void loadFonts(QString dirStr);
  bool loadCharts(QString chartsXml);
  void spawnPreferences();
  void spawnJobRunner();
  void updateFromConfig();
  QList<AbstractChart*> charts;

  // Automatic hibernation timer shutting down monitor signal after X minutes of inactivity
  QTimer hiberTimer;

  // Timer that resets all charts after being idle for configured time
  QTimer resetTimer;

  // Cooldown for switching monitor on or off using 'q' to allow hibernation to complete
  QTimer hiberCooldownTimer;
  bool allowHibernate = true;
};
#endif/*__OCCURITY_MAINWINDOW_H__*/
