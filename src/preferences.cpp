/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            preferences.cpp
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

#include "preferences.h"
#include "about.h"
#include "slider.h"
#include "combobox.h"
#include "checkbox.h"
#include "version.h"

#include <QApplication>
#include <stdio.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QApplication>
#include <QScrollArea>
#include <QTabWidget>
#include <QDialogButtonBox>

Preferences::Preferences(QSettings &config, const QList<AbstractChart *> charts, QWidget *parent)
  : QDialog(parent), config(config)
{
  setFixedSize(1200, 850);
  setWindowIcon(QIcon(":icon.png"));
  setWindowTitle(QApplication::applicationName() + QString(" v%1.%2.%3").arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH) + " preferences");

  About *tabWidget = new About(this);
  tabWidget->setFocusPolicy(Qt::NoFocus);
  tabWidget->setMinimumWidth(600);

  Slider *rulerWidth = new Slider(config, "main", "rulerWidth", tr("Physical length of ruler (mm):"), 50, 800, 138, 1, this);
  rulerWidth->setFocus();
  QLabel *rulerLabel = new QLabel(this);
  rulerLabel->setPixmap(QPixmap(":ruler.png"));
  Slider *patientDistance = new Slider(config, "main", "patientDistance", tr("Patient distance to monitor (cm):"), 50, 1000, 600, 10, this);

  ComboBox *startingChart = new ComboBox(config, "main", "startingChart", tr("Starting chart:"), "", this);
  for(const auto *chart: charts) {
    startingChart->addConfigItem(chart->objectName(), chart->objectName());
  }

  Slider *minutesBeforeSizeReset = new Slider(config, "main", "minutesBeforeSizeReset", tr("Idle time before size reset (minutes):"), 1, 60, 5, 1, this);
  Slider *minutesBeforeSleep = new Slider(config, "main", "minutesBeforeSleep", tr("Idle time before putting the monitor to sleep (minutes):"), 5, 480, 120, 5, this);
  Slider *hexRed = new Slider(config, "main", "redValue", tr("Red color value:"), 0, 255, 210, 1, this);
  Slider *hexGreen = new Slider(config, "main", "greenValue", tr("Green color value:"), 0, 255, 210, 1, this);
  Slider *rowSkipDelta = new Slider(config, "main", "rowSkipDelta", tr("Skip this many lines when using multiline row skipping:"), 2, 10, 4, 1, this);

  CheckBox *useRowCaptions = new CheckBox(config, "main", "useRowCaptions", tr("Use row captions for optotype charts"), false, this);
  CheckBox *showMouseCursor = new CheckBox(config, "touch", "showMouse", tr("Show mouse cursor"), false, this);
  CheckBox *showTouchControls = new CheckBox(config, "touch", "touchControls", tr("Show touch controls (requires restart)"), false, this);
  CheckBox *leftHandedOperator = new CheckBox(config, "touch", "leftHandedOperator", tr("Left handed operator"), false, this);

  QVBoxLayout *configLayout = new QVBoxLayout;
  configLayout->addWidget(rulerWidth);
  configLayout->addWidget(rulerLabel, 0, Qt::AlignCenter);
  configLayout->addWidget(patientDistance);
  configLayout->addWidget(startingChart);
  configLayout->addWidget(minutesBeforeSizeReset);
  configLayout->addWidget(minutesBeforeSleep);
  configLayout->addWidget(hexRed);
  configLayout->addWidget(hexGreen);
  configLayout->addWidget(rowSkipDelta);
  configLayout->addWidget(useRowCaptions);
  configLayout->addWidget(showMouseCursor);
  configLayout->addWidget(showTouchControls);
  configLayout->addWidget(leftHandedOperator);

  QHBoxLayout *hLayout = new QHBoxLayout;
  hLayout->addLayout(configLayout);
  hLayout->addWidget(tabWidget);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(hLayout);
  setStyleSheet("QPushButton {"
                "background-color: #b2b2b2;"
                "border-style: outset;"
                "border-width: 6px;"
                "border-radius: 15px;"
                "border-color: black;"
                "font: bold 60px;"
                "padding: 6px;"
                "}");
  QDialogButtonBox *closeButton = new QDialogButtonBox(QDialogButtonBox::Ok, this);
  connect(closeButton, &QDialogButtonBox::accepted, this, &QDialog::accept);
  layout->addWidget(closeButton);

  setLayout(layout);

  installEventFilter(this);
}

Preferences::~Preferences()
{
}

bool Preferences::eventFilter(QObject *, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent->key() == Qt::Key_Up) {
      focusPreviousChild();
      return true;
    } else if(keyEvent->key() == Qt::Key_Down) {
      focusNextChild();
      return true;
    } else if(keyEvent->key() == Qt::Key_Enter ||
              keyEvent->key() == Qt::Key_Return ||
              keyEvent->key() == Qt::Key_R ||
              keyEvent->key() == Qt::Key_P) { // 'R' is the default enter key on the remote
      accept();
      return true;
    }
  }
  return false;
}
