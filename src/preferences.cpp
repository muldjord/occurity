/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            preferences.cpp
 *
 *  Tue Mar 13 17:00:00 UTC+1 2018
 *  Copyright 2018 Lars Bisballe
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

#include "preferences.h"
#include "about.h"
#include "slider.h"

#include <stdio.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollArea>
#include <QTabWidget>

Preferences::Preferences(QSettings *config, QWidget *parent) : QDialog(parent)
{
  this->config = config;

  setFixedSize(1200, 400);
  setWindowIcon(QIcon(":icon.png"));
  setWindowTitle("VisuTest v" VERSION);

  move(QApplication::desktop()->width() / 2 - 225, QApplication::desktop()->height() / 2 - 250);

  About *tabWidget = new About(this);
  tabWidget->setMinimumWidth(600);

  QLabel *rulerWidthLabel = new QLabel(tr("Physical length of ruler (mm):"), this);
  Slider *rulerWidth = new Slider(config, "", "rulerWidth", 50, 800, 137, 1, this);
  rulerWidth->setFocus();
  QLabel *rulerLabel = new QLabel(this);
  rulerLabel->setPixmap(QPixmap(":ruler.png"));
  QLabel *patientDistanceLabel = new QLabel(tr("Patient distance to monitor (cm):"), this);
  Slider *patientDistance = new Slider(config, "", "patientDistance", 50, 1000, 600, 10, this);
  QLabel *sizeResetTimeLabel = new QLabel(tr("Idle time before size reset (seconds):"), this);
  Slider *sizeResetTime = new Slider(config, "", "sizeResetTime", 10, 3600, 240, 10, this);
  QLabel *hexRedLabel = new QLabel(tr("Red color value:"), this);
  Slider *hexRed = new Slider(config, "", "redValue", 0, 255, 210, 1, this);
  QLabel *hexGreenLabel = new QLabel(tr("Green color value:"), this);
  Slider *hexGreen = new Slider(config, "", "greenValue", 0, 255, 210, 1, this);
  QPushButton *saveButton = new QPushButton(tr("Close"), this);
  connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);

  QVBoxLayout *configLayout = new QVBoxLayout;
  configLayout->addWidget(rulerWidthLabel);
  configLayout->addWidget(rulerWidth);
  configLayout->addWidget(rulerLabel);
  configLayout->addWidget(patientDistanceLabel);
  configLayout->addWidget(patientDistance);
  configLayout->addWidget(sizeResetTimeLabel);
  configLayout->addWidget(sizeResetTime);
  configLayout->addWidget(hexRedLabel);
  configLayout->addWidget(hexRed);
  configLayout->addWidget(hexGreenLabel);
  configLayout->addWidget(hexGreen);
  configLayout->addWidget(saveButton);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addLayout(configLayout);
  layout->addWidget(tabWidget);

  setLayout(layout);

  installEventFilter(this);
}

Preferences::~Preferences()
{
}

bool Preferences::eventFilter(QObject *, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent->key() == Qt::Key_Up) {
      focusPreviousChild();
      return true;
    } else if(keyEvent->key() == Qt::Key_Down) {
      focusNextChild();
      return true;
    } else if(keyEvent->key() == Qt::Key_R) {
      accept();
      return true;
    }
  }
  return false;
}
