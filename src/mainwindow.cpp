/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.cpp
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

#include "mainwindow.h"
#include "preferences.h"

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QSettings>
#include <QScreen>
#include <QTimer>
#include <QDir>
#include <QProcess>
#include <QDomDocument>

MainWindow::MainWindow()
{
  setCursor(Qt::BlankCursor);
  printf("Running VisuTest v." VERSION "\n");
  config = new QSettings("config.ini", QSettings::IniFormat);
  mainSettings = new MainSettings;
  setWindowTitle("VisuTest v" VERSION);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  showFullScreen();
  qApp->processEvents();

  QRect res = QGuiApplication::primaryScreen()->geometry();
  mainSettings->width = res.width();
  mainSettings->height = res.height();
  printf("  Monitor resolution: %d x %d\n", mainSettings->width, mainSettings->height);
  updateFromConfig();

  if(!QFile::exists(mainSettings->chartsXml)) {
    QFile::copy("charts.xml.example", mainSettings->chartsXml);
  }

  if(loadCharts(mainSettings->chartsXml)) {
    printf("Charts xml loaded successfully!\n");
  } else {
    printf("Charts xml loading failed!\n");
    exit(1);
  }

  installEventFilter(this);

  secretTimer.setInterval(400);
  secretTimer.setSingleShot(true);
  connect(&secretTimer, &QTimer::timeout, this, &MainWindow::enableSecret);

  hiberCooldownTimer.setInterval(10000);
  hiberCooldownTimer.setSingleShot(true);
  connect(&hiberCooldownTimer, &QTimer::timeout, this, &MainWindow::enableHibernate);

  hiberTimer.setInterval(mainSettings->hibernateTime);
  hiberTimer.setSingleShot(true);
  connect(&hiberTimer, &QTimer::timeout, this, &MainWindow::hibernate);
  hiberTimer.start();

  QTimer::singleShot(0, this, &MainWindow::init);
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
  if(!charts.isEmpty()) {
    setScene(charts.first());
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
      chart->setObjectName(xmlChart.attribute("caption"));
      if(numKey) {
        // 48 is the Qt::Key equivalent of Qt::Key_0 ascending from there to Qt::Key_9
        chart->setNumKey((Qt::Key)(numKey + 48));
      }
      chart->setBgColor(xmlChart.attribute("bgcolor"));
      printf("  Parsing chart: '%s' with type '%s':\n", chart->objectName().toStdString().c_str(), chart->getType().toStdString().c_str());
      if(chart->getType() == "optotype") {
        if(xmlChart.hasAttribute("sizelock") && xmlChart.attribute("sizelock") == "true") {
          printf("    Size lock: true\n");
          chart->setSizeLocked(true);
        } else {
          printf("    Size lock: false\n");
        }
        chart->setOptotype(xmlChart.attribute("optotype"));
        printf("    Optotype: '%s'\n", chart->getOptotype().toStdString().c_str());
        if(xmlChart.hasAttribute("crowdingspan")) {
          chart->setCrowdingSpan(xmlChart.attribute("crowdingspan").toDouble());
        }
        printf("    Crowding span: '%f'\n", chart->getCrowdingSpan());
        QDomNodeList xmlRows = xmlChart.elementsByTagName("row");
        QMap<QString, int> rowSizeMap;
        for(int b = 0; b < xmlRows.count(); ++b) {
          QDomElement xmlRow = xmlRows.at(b).toElement();
          QString rowSize = xmlRow.attribute("size");
          QString rowChars = xmlRow.text();
          chart->addRow(rowSize, rowChars);
          printf("    Row: '%s', size '%s'\n", xmlRow.text().toStdString().c_str(), xmlRow.attribute("size").toStdString().c_str());
        }
        QList<QString> rowSizeStrings;
        for(auto &str : rowSizeMap.keys()) {
          rowSizeStrings.append(str);
        }
        //chart->setRowSizes(rowSizeStrings);
        if(xmlChart.hasAttribute("startsize")) {
          chart->setStartSize(xmlChart.attribute("startsize"));
        }
      } else if(chart->getType() == "svg") {
        chart->setSource(xmlChart.attribute("source"));
        if(xmlChart.hasAttribute("scaling")) {
          chart->setScaling(xmlChart.attribute("scaling"));
          printf("    Scaling: %s\n", xmlChart.attribute("scaling").toStdString().c_str());
        }
        QDomNodeList xmlSvgLayers = xmlChart.elementsByTagName("layer");
        for(int b = 0; b < xmlSvgLayers.count(); ++b) {
          QDomElement xmlSvgLayer = xmlSvgLayers.at(b).toElement();
          QString svgLayerId = xmlSvgLayer.attribute("id");
          if(!chart->addSvgLayer(svgLayerId)) {
            printf("  Couldn't add svg layer with id '%s'\n", svgLayerId.toStdString().c_str());
          } else {
            printf("    SVG layer id: '%s'\n", svgLayerId.toStdString().c_str());
          }
        }
      }
      chart->init();
      charts.append(chart);
    }
  }
  printf("\n");

  return true;
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    // Reset hibernation inactivity timer
    hiberTimer.start();

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // Check for secret "2018" shutdown sequence check
    if(allowSecret) {
      if(keyEvent->key() == Qt::Key_2 && secretState == 0) {
        secretState = 1;
      } else if(keyEvent->key() == Qt::Key_0 && secretState == 1) {
        secretState = 2;
      } else if(keyEvent->key() == Qt::Key_1 && secretState == 2) {
        secretState = 3;
      } else if(keyEvent->key() == Qt::Key_8 && secretState == 3) {
        printf("Shutdown requested by user, initiating shutdown...\n");
        QProcess::execute("bash", QStringList({"./scripts/shutdown.sh"}));
      } else {
        secretState = 0;
      }
      allowSecret = false;
      secretTimer.start();
    }

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
      setScene(charts.at(chosen));
      printf("Chart '%s' activated!\n", charts.at(chosen)->objectName().toStdString().c_str());
      return true;
    }
    if(keyEvent->key() == Qt::Key_P) {
      spawnPreferences();
      updateFromConfig();
      return true;
    } else if(keyEvent->key() == Qt::Key_Q) {
      if(allowHibernate) {
        flipHibernate();
        return true;
      }
    }
  }
  return false;
}

void MainWindow::updateFromConfig()
{
  while(!config->contains("rulerWidth")) {
    if(config->contains("physHeight")) {
      config->setValue("rulerWidth", ((double)config->value("physHeight").toInt() / (double)mainSettings->height) * 500);
      config->remove("physHeight");
    }
    spawnPreferences();
  }

  // Convert obsolete 'physDistance' variable to 'patientDistance'
  if(config->contains("physDistance")) {
    config->setValue("patientDistance", config->value("physDistance").toInt());
    config->remove("physDistance");
  }

  if(!config->contains("chartsXml")) {
    config->setValue("chartsXml", "charts.xml");
  }
  if(!config->contains("optotypesDir")) {
    config->setValue("optotypesDir", "./optotypes");
  }
  if(!config->contains("sizeResetTime")) {
    config->setValue("sizeResetTime", 240);
  }
  if(!config->contains("hibernateTime")) {
    config->setValue("hibernateTime", 120);
  }

  mainSettings->sizeResetTime = config->value("sizeResetTime").toInt() * 1000; // Seconds
  mainSettings->hibernateTime = config->value("hibernateTime").toInt() * 1000 * 60; // Minutes

  mainSettings->patientDistance = config->value("patientDistance").toDouble(); // Cm
  mainSettings->rulerWidth = config->value("rulerWidth").toDouble(); // Mm

  mainSettings->distanceFactor = mainSettings->patientDistance / 600.0;
  mainSettings->pxPerMm = 500.0 / mainSettings->rulerWidth;
  // At 6 m distance (size 0.1) a letter should be 87.3 mm tall on screen (5 arc minutes)
  mainSettings->pxPerArcMin = (87.3 / 5.0) * mainSettings->pxPerMm;
  mainSettings->hexRed = "#" + QString::number(config->value("redValue").toInt(), 16) + "0000";
  mainSettings->hexGreen = "#00" + QString::number(config->value("greenValue").toInt(), 16) + "00";

  mainSettings->optotypesDir = config->value("optotypesDir").toString();
  mainSettings->chartsXml = config->value("chartsXml").toString();

  printf("  Pixels per mm: %f\n", mainSettings->pxPerMm);
  printf("  Pixels per arc minute: %f\n", mainSettings->pxPerArcMin);
  printf("\n");
  emit configUpdated();
}

void MainWindow::spawnPreferences()
{
  printf("Spawning Preferences...\n");
  Preferences prefs(config, this);
  prefs.exec();
}

void MainWindow::enableHibernate()
{
  printf("Now allowing hibernate again\n");
  allowHibernate = true;
}

void MainWindow::enableSecret()
{
  allowSecret = true;
}

void MainWindow::hibernate()
{
  flipHibernate(true);
}

void MainWindow::flipHibernate(bool forceHibernate)
{
  int exitState = QProcess::execute("bash", QStringList({"./scripts/displaystate.sh"}));
  // On error exitState is -1 or -2. If display is on it is 42. All other cases it is 0.
  if(exitState >= 0) {
    if(exitState == 42) {
      QProcess::execute("bash", QStringList({"./scripts/hibernate.sh"}));
    } else if(!forceHibernate) {
      QProcess::execute("bash", QStringList({"./scripts/wakeup.sh"}));
    }
  }
  // Disallow new hibernation for a while, as it queue's each time allowing for some
  // weird behaviour.
  allowHibernate = false;
  hiberCooldownTimer.start();
}
