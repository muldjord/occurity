/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.cpp
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

#include "mainwindow.h"
#include "pindialog.h"
#include "preferences.h"
#include "jobrunner.h"
#include "version.h"

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QDir>
#include <QProcess>
#include <QDomDocument>

bool videoPlaybackSupported()
{
  QFile f("/proc/device-tree/model");
  if(f.exists()) {
    if(f.open(QIODevice::ReadOnly)) {
      QByteArray model = f.readAll();
      if(model.left(13) == "Raspberry Pi ") {
        if(model.mid(13, 1).toInt() >= 4) {
          printf("Detected Raspberry Pi 4 or higher, enabling video playback...\n\n");
          return true;
        }
      } else {
        printf("'/proc/device-tree/model' detected, but apparently not a Raspberry Pi.\n");
        printf("Assuming user wants video playback so enabling it...\n\n");
        return true;
      }
    }
  } else {
    printf("'/proc/device-tree/model' doesn't exist, maybe not running on a Yocto image?\n");
    printf("Assuming user wants video playback so enabling it...\n\n");
    return true;
  }
  return false;
}

MainWindow::MainWindow(QSettings &config) : config(config)
{
  printf("Running %s\n", qPrintable(QApplication::applicationName() + QString(" v%1.%2.%3").arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH)));
  setWindowTitle(QApplication::applicationName() + QString(" v%1.%2.%3").arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH));

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  showFullScreen();
  qApp->processEvents();

  QRect res = QGuiApplication::primaryScreen()->geometry();
  mainSettings.width = res.width();
  mainSettings.height = res.height();
  printf("  Monitor resolution: %d x %d\n", mainSettings.width, mainSettings.height);

  updateFromConfig();

  if(!QFile::exists(mainSettings.chartsXml)) {
    QFile::copy("charts.xml.example", mainSettings.chartsXml);
  }

  if(loadCharts(mainSettings.chartsXml)) {
    printf("Charts xml loaded successfully!\n\n");
  } else {
    printf("Charts xml loading failed!\n\n");
    exit(1);
  }

  installEventFilter(this);

  if(mainSettings.enableVideoPlayer && videoPlaybackSupported()) {
    videoPlayer = new VideoPlayer(mainSettings.videosFolder,
                                  mainSettings.width, mainSettings.height,
                                  this);
  }

  // Initial interval was set in updateFromConfig;
  sleepTimer.setSingleShot(true);
  connect(&sleepTimer, &QTimer::timeout, this, &MainWindow::monitorSleep);
  sleepTimer.start();

  delayedSleepTimer.setInterval(1000);
  delayedSleepTimer.setSingleShot(true);
  connect(&delayedSleepTimer, &QTimer::timeout, this, &MainWindow::monitorSleep);

  delayedMonitorOnTimer.setInterval(1000);
  delayedMonitorOnTimer.setSingleShot(true);
  connect(&delayedMonitorOnTimer, &QTimer::timeout, this, &MainWindow::monitorOn);

  // Initial interval was set in updateFromConfig;
  resetTimer.setSingleShot(true);
  resetTimer.start();

  QTimer::singleShot(100, this, &MainWindow::init);
}

MainWindow::~MainWindow()
{
  if(videoPlayer != nullptr) {
    delete videoPlayer;
  }
}

void MainWindow::init()
{
  while(!config.contains("main/rulerWidth")) {
    spawnPreferences();
  }

  if(!charts.isEmpty()) {
    if(mainSettings.startingChart.isEmpty()) {
      setScene(charts.first());
    } else {
      bool foundChart = false;
      for(auto *chart: charts) {
        if(chart->objectName() == mainSettings.startingChart) {
          setScene(chart);
          foundChart = true;
          break;
        }
      }
      if(!foundChart) {
        setScene(charts.first());
      }
    }
  }
}

bool MainWindow::loadCharts(QString chartsXml)
{
  printf("Loading charts from file: '%s'\n", chartsXml.toStdString().c_str());

  QFile xmlFile(chartsXml);
  QByteArray xmlData;
  if(xmlFile.open(QIODevice::ReadOnly)) {
    xmlData = xmlFile.readAll();
    xmlFile.close();
  } else {
    return false;
  }

  QDomDocument xmlDoc;
  if(!xmlDoc.setContent(xmlData)) {
    return false;
  }

  QDomNodeList groupNodes = xmlDoc.documentElement().elementsByTagName("group");
  for(int a = 0; a < groupNodes.count(); ++a) {
    QDomElement xmlGroup = groupNodes.at(a).toElement();
    int numKey = -1;
    QString touchControlsGroup = xmlGroup.attribute("touchcontrols").toLower().simplified().replace(" ", "");
    if(xmlGroup.hasAttribute("numkey")) {
      bool isInt = false;
      int tmpNumKey = xmlGroup.attribute("numkey").toInt(&isInt);
      if(isInt && tmpNumKey >= 0 && tmpNumKey <= 9) {
        numKey = tmpNumKey;
      }
    }
    printf("Parsing group: Key '%d':\n", numKey);
    QDomNodeList chartNodes = xmlGroup.toElement().elementsByTagName("chart");
    for(int b = 0; b < chartNodes.count(); ++b) {
      QDomElement xmlChart = chartNodes.at(b).toElement();
      AbstractChart *chart = nullptr;
      QString chartType = xmlChart.attribute("type");
      if(chartType == "optotype") {
        chart = new OptotypeChart(mainSettings, this);
      } else if(chartType == "svg") {
        chart = new SvgChart(mainSettings, this);
      }
      if(chart == nullptr) {
        printf("PARSE ERROR: Chart type '%s' was not recognized, skipping...\n",
               chartType.toStdString().c_str());
        continue;
      }
      chart->installEventFilter(this);
      chart->setType(chartType);
      connect(this, &MainWindow::configUpdated, chart, &AbstractChart::updateAll);
      connect(this, &MainWindow::configUpdated, chart, &AbstractChart::updateTouchControls);
      connect(&resetTimer, &QTimer::timeout, chart, &AbstractChart::resetAll);
      chart->setObjectName(xmlChart.attribute("caption"));
      if(numKey) {
        // 48 is the Qt::Key equivalent of Qt::Key_0 ascending from there to Qt::Key_9
        chart->setNumKey((Qt::Key)(numKey + 48));
      }
      chart->setBgColor(xmlChart.attribute("bgcolor"));
      QString touchControlsChart = xmlChart.attribute("touchcontrols").toLower().simplified().replace(" ", "");
      chart->setTouchControls(touchControlsChart + "," + touchControlsGroup);
      printf("  Parsing chart: '%s' with type '%s':\n", chart->objectName().toStdString().c_str(), chart->getType().toStdString().c_str());
      if(chart->getType() == "optotype") {
        if(xmlChart.hasAttribute("sizelock") && xmlChart.attribute("sizelock") == "true") {
          printf("    Size lock: true\n");
          static_cast<OptotypeChart*>(chart)->setSizeLocked(true);
        } else {
          printf("    Size lock: false\n");
        }
        static_cast<OptotypeChart*>(chart)->setOptotype(xmlChart.attribute("optotype"));
        printf("    Optotype: '%s'\n", static_cast<OptotypeChart*>(chart)->getOptotype().toStdString().c_str());
        if(xmlChart.hasAttribute("crowdingspan")) {
          static_cast<OptotypeChart*>(chart)->setCrowdingSpan(xmlChart.attribute("crowdingspan").toDouble());
        }
        if(xmlChart.hasAttribute("animation")) {
          static_cast<OptotypeChart*>(chart)->setAnimation(xmlChart.attribute("animation"));
        }
        printf("    Crowding span: '%f'\n", static_cast<OptotypeChart*>(chart)->getCrowdingSpan());
        if(xmlChart.hasAttribute("startsize")) {
          static_cast<OptotypeChart*>(chart)->setStartSize(xmlChart.attribute("startsize"));
        }
        if(xmlChart.hasAttribute("fadetimings")) {
          static_cast<OptotypeChart*>(chart)->setFadeTimings(xmlChart.attribute("fadetimings"));
        }
        if(xmlChart.hasAttribute("fadelevels")) {
          static_cast<OptotypeChart*>(chart)->setFadeLevels(xmlChart.attribute("fadelevels"));
        }
        QDomNodeList xmlRows = xmlChart.elementsByTagName("row");
        QMap<QString, int> rowSizeMap;
        for(int b = 0; b < xmlRows.count(); ++b) {
          QDomElement xmlRow = xmlRows.at(b).toElement();
          QString rowSize = xmlRow.attribute("size");
          QString rowCaption = xmlRow.attribute("caption");
          QString rowChars = xmlRow.text();
          static_cast<OptotypeChart*>(chart)->addRow(rowSize, rowCaption, rowChars);
          printf("    Row: '%s', size '%s', caption '%s'\n", qPrintable(rowChars), qPrintable(rowSize), qPrintable(rowCaption));
        }
        QList<QString> rowSizeStrings;
        for(auto &str : rowSizeMap.keys()) {
          rowSizeStrings.append(str);
        }
      } else if(chart->getType() == "svg") {
        static_cast<SvgChart*>(chart)->setSource(xmlChart.attribute("source"));
        if(xmlChart.hasAttribute("scaling")) {
          static_cast<SvgChart*>(chart)->setScaling(xmlChart.attribute("scaling"));
          printf("    Scaling: %s\n", xmlChart.attribute("scaling").toStdString().c_str());
        }
        QDomNodeList xmlSvgLayers = xmlChart.elementsByTagName("layer");
        for(int b = 0; b < xmlSvgLayers.count(); ++b) {
          QDomElement xmlSvgLayer = xmlSvgLayers.at(b).toElement();
          QString svgLayerId = xmlSvgLayer.attribute("id");
          if(!static_cast<SvgChart*>(chart)->addSvgLayer(svgLayerId)) {
            printf("  Couldn't add svg layer with id '%s'\n", svgLayerId.toStdString().c_str());
          } else {
            printf("    SVG layer id: '%s'\n", svgLayerId.toStdString().c_str());
          }
        }
      }
      chart->init();
      chart->updateTouchControls();
      charts.append(chart);
    }
  }
  printf("\n");

  return true;
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    // Reset sleep timer
    sleepTimer.start();

    // Restart reset timer
    resetTimer.start();

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    QList<int> chartPool;
    AbstractChart *currentChart = nullptr;
    for(int a = 0; a < charts.length(); ++a) {
      if(keyEvent->key() == charts.at(a)->getNumKey()) {
        chartPool.append(a);
      }
      if(charts.at(a) == scene()) {
        currentChart = charts.at(a);
      }
    }
    if(currentChart == nullptr) {
      qWarning("One or more charts have no recognized type!\n");
    }
    // Set size for all other charts on same button if sizeLock is set
    for(int a = 0; a < charts.length(); ++a) {
      if(currentChart != nullptr && currentChart != charts.at(a) &&
         charts.at(a)->getType() == "optotype" &&
         charts.at(a)->isSizeLocked() &&
         currentChart->isSizeLocked()) {
        charts.at(a)->setSize(currentChart->getSize());
      }
    }
    if(!chartPool.isEmpty()) {
      int chosen = chartPool.first();
      if(chartPool.length() != 1) {
        for(int a = 0; a < chartPool.length(); ++a) {
          if(scene()->objectName() == charts.at(chartPool.at(a))->objectName()) {
            if(a + 1 < chartPool.length()) {
              chosen = chartPool.at(a + 1);
              break;
            }
          }
        }
      }
      currentChart->makeIdle();
      setScene(charts.at(chosen));
      printf("Chart '%s' activated!\n", charts.at(chosen)->objectName().toStdString().c_str());
      return true;
    }
    if(keyEvent->key() == Qt::Key_P) {
      spawnPreferences();
      // updateFromConfig run from spawnPreferences
      return true;
    } else if(keyEvent->key() == Qt::Key_J) {
      spawnJobRunner();
      return true;
    } else if(keyEvent->key() == Qt::Key_Q) {
      if(monitorIsOn) {
        delayedSleepTimer.start();
      } else {
        delayedMonitorOnTimer.start();
      }
      return true;
    } else if(keyEvent->key() == Qt::Key_S) {
      if(videoPlayer != nullptr) {
        videoPlayer->show();
        videoPlayer->setFocus();
        videoPlayer->playPressed();
      }
      return true;
    }
    monitorIsOn = true;
  }
  return false;
}

void MainWindow::updateFromConfig()
{
  // Main
  if(!config.contains("main/chartsXml")) {
    config.setValue("main/chartsXml", "charts.xml");
  }
  if(!config.contains("main/startingChart")) {
    config.setValue("main/startingChart", "Sloan 1");
  }
  if(!config.contains("main/minutesBeforeSizeReset")) {
    config.setValue("main/minutesBeforeSizeReset", 5);
  }
  if(!config.contains("main/minutesBeforeSleep")) {
    config.setValue("main/minutesBeforeSleep", 120);
  }
  if(!config.contains("main/rowSkipDelta")) {
    config.setValue("main/rowSkipDelta", 4);
  }
  if(!config.contains("main/useRowCaptions")) {
    config.setValue("main/useRowCaptions", false);
  }
  if(!config.contains("main/pinCode")) {
    config.setValue("main/pinCode", "4242");
  }
  if(!config.contains("main/enableVideoPlayer")) {
    config.setValue("main/enableVideoPlayer", true);
  }

  // Touch controls
  if(!config.contains("touch/touchControls")) {
    config.setValue("touch/touchControls", false);
  }
  if(!config.contains("touch/leftHandedOperator")) {
    config.setValue("touch/leftHandedOperator", false);
  }
  if(!config.contains("touch/showMouse")) {
    config.setValue("touch/showMouse", false);
  }

  // Folders
  if(!config.contains("folders/optotypes")) {
    config.setValue("folders/optotypes", "./optotypes");
  }
  if(!config.contains("folders/jobs")) {
    config.setValue("folders/jobs", "./jobs");
  }
  if(!config.contains("folders/videos")) {
    config.setValue("folders/videos", "./videos");
  }

  resetTimer.setInterval(config.value("main/minutesBeforeSizeReset").toInt() * 1000 * 60); // Minutes
  resetTimer.start();

  sleepTimer.setInterval(config.value("main/minutesBeforeSleep").toInt() * 1000 * 60); // Minutes
  // Don't start sleepTimer here, just set the interval. The next keypress will start it.

  mainSettings.rowSkipDelta = config.value("main/rowSkipDelta").toInt();

  mainSettings.useRowCaptions = config.value("main/useRowCaptions").toBool();

  mainSettings.enableVideoPlayer = config.value("main/enableVideoPlayer").toBool();

  mainSettings.touchControls = config.value("touch/touchControls").toBool();
  mainSettings.leftHandedOperator = config.value("touch/leftHandedOperator").toBool();
  if(config.value("touch/showMouse", false).toBool()) {
    setCursor(Qt::ArrowCursor);
  } else {
    setCursor(Qt::BlankCursor);
  }

  mainSettings.pinCode = config.value("main/pinCode").toString();

  mainSettings.optotypesFolder = config.value("folders/optotypes").toString();

  mainSettings.jobsFolder = config.value("folders/jobs").toString();

  mainSettings.videosFolder = config.value("folders/videos").toString();

  mainSettings.patientDistance = config.value("main/patientDistance").toDouble(); // Cm
  mainSettings.rulerWidth = config.value("main/rulerWidth").toDouble(); // Mm

  mainSettings.distanceFactor = mainSettings.patientDistance / 600.0;
  mainSettings.pxPerMm = 500.0 / mainSettings.rulerWidth;
  // At 6 m distance (size 0.1) a letter should be 87.3 mm tall on screen (5 arc minutes)
  mainSettings.pxPerArcMin = (87.3 / 5.0) * mainSettings.pxPerMm;
  mainSettings.hexRed = "#" + QString::number(config.value("main/redValue").toInt(), 16) + "0000";
  mainSettings.hexGreen = "#00" + QString::number(config.value("main/greenValue").toInt(), 16) + "00";

  mainSettings.chartsXml = config.value("main/chartsXml").toString();
  mainSettings.startingChart = config.value("main/startingChart").toString();

  printf("  Pixels per mm: %f\n", mainSettings.pxPerMm);
  printf("  Pixels per arc minute: %f\n", mainSettings.pxPerArcMin);
  printf("\n");
  emit configUpdated();
}

void MainWindow::spawnPreferences()
{
  PinDialog pinDialog(mainSettings.pinCode, mainSettings.touchControls, this);
  if(pinDialog.exec() == QDialog::Accepted) {
    printf("Spawning Preferences...\n");
    Preferences prefs(config, charts, this);
    prefs.exec();
    activateWindow();
    // ALWAYS run updateFromConfig after changing preferences to make sure new values are set in charts
    updateFromConfig();
  }
}

void MainWindow::spawnJobRunner()
{
  PinDialog pinDialog(mainSettings.pinCode, mainSettings.touchControls, this);
  if(pinDialog.exec() == QDialog::Accepted) {
    printf("Spawning job runner...\n");
    JobRunner jobRunner(mainSettings, this);
    jobRunner.exec();
    activateWindow();
  }
}

void MainWindow::monitorSleep()
{
  QProcess::execute("bash", {"./scripts/sleep.sh"});
  monitorIsOn = false;
}

void MainWindow::monitorOn()
{
  monitorIsOn = true;
}
