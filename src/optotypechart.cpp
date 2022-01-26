/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            optotypechart.cpp
 *
 *  Tue Jan 11 14:32:00 UTC+1 2021
 *  Copyright 2021 Lars Bisballe
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

#include "optotypechart.h"

#include <stdio.h>
#include <math.h>
#include <QKeyEvent>
#include <QGraphicsSvgItem>
#include <QFileInfo>
#include <QMessageBox>

OptotypeChart::OptotypeChart(MainSettings *mainSettings, QObject *parent) :
  AbstractChart(mainSettings, parent)
{
  sizeResetTimer.setInterval(mainSettings->sizeResetTime);
  sizeResetTimer.setSingleShot(true);
  connect(&sizeResetTimer, &QTimer::timeout, this, &OptotypeChart::resetSize);
}

OptotypeChart::~OptotypeChart()
{
}

void OptotypeChart::init()
{
  crowdRect = addRect(0, 0, 10, 10);
  
  sizeItem = new QGraphicsSimpleTextItem("0.0");
  copyrightItem = new QGraphicsSimpleTextItem("");
  QFont font;
  font.setFamily("Arial");
  font.setPixelSize((mainSettings->pxPerArcMin * 5.0) * mainSettings->distanceFactor * 0.2);
  sizeItem->setFont(font);
  font.setPixelSize(25);
  copyrightItem->setFont(font);
  if(QFile::exists("optotypes/copyrights.txt")) {
    QFile copyrightFile("optotypes/copyrights.txt");
    if(copyrightFile.open(QIODevice::ReadOnly)) {
      while(!copyrightFile.atEnd()) {
        QList<QByteArray> copyright = copyrightFile.readLine().split(';');
        if(!copyright.isEmpty() && optotype == copyright.first()) {
          copyrightItem->setText(copyright.last().trimmed());
          break;
        }
      }
      copyrightFile.close();
    }
  }
  titleItem->setText(objectName());

  addItem(sizeItem);
  addItem(titleItem);
  addItem(copyrightItem);
  copyrightItem->setPos((mainSettings->width / 2.0) - (copyrightItem->boundingRect().width() / 2.0), mainSettings->height - 35);

  setSize(startSize);
}

void OptotypeChart::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Up) {
    currentRowIdx--;
    if(currentRowIdx < 0) {
      currentRowIdx = 0;
    }
  } else if(event->key() == Qt::Key_Down) {
    currentRowIdx++;
    if(currentRowIdx > rows.length() - 1) {
      currentRowIdx = rows.length() - 1;
    }
  } else if(event->key() == Qt::Key_Left) {
    if(skew > - (rows.at(currentRowIdx).second->childItems().length() / 2)) {
      skew--;
    }
  } else if(event->key() == Qt::Key_Right) {
    if(skew < rows.at(currentRowIdx).second->childItems().length() / 2) {
      skew++;
    }
  } else if(event->key() == Qt::Key_C) {
    mainSettings->crowding = !mainSettings->crowding;
  } else if(event->key() == Qt::Key_M) {
    mainSettings->single = !mainSettings->single;
  } else if(event->key() == Qt::Key_R) {
    // Shuffle current row, but remember to reset when changing row
  }

  // Update reset interval in case it has changed
  sizeResetTimer.setInterval(mainSettings->sizeResetTime);
  sizeResetTimer.start();

  updateAll();
}

void OptotypeChart::setOptotype(const QString &optotype)
{
  this->optotype = optotype;
}

QString OptotypeChart::getOptotype()
{
  return optotype;
}

void OptotypeChart::setCrowdingSpan(const double &crowdingSpan)
{
  this->crowdingSpan = crowdingSpan;
}

double OptotypeChart::getCrowdingSpan()
{
  return crowdingSpan;
}

void OptotypeChart::setSizeLocked(const bool &sizeLocked)
{
  this->sizeLocked = sizeLocked;
}

bool OptotypeChart::isSizeLocked()
{
  return sizeLocked;
}

void OptotypeChart::addRow(const QString &size, const QString &row)
{
  /*
    Don't do any resizing / scaling here. It has to be done in updateAll(), otherwise it
    won't be re-scaled when preferences are changed.
    Here we simply load all SVG's into QGraphicsSvgItems, move them into place and add them
    to QGraphicsItemGroups.
  */
  QPair<QString, QGraphicsItemGroup *> rowPair;

  QGraphicsSvgItem *svgSpace = new QGraphicsSvgItem(mainSettings->optotypesDir + "/" + optotype + "/_.svg");
  spaceWidth = svgSpace->boundingRect().width();
  
  QGraphicsItemGroup *layer = new QGraphicsItemGroup;
  addItem(layer);

  int curX = 0;
  QList<QString> letters;
  if(row.contains(";")) {
    for(const auto &letter: row.split(";")) {
      letters.append(letter);
    }
  } else {
    for(const auto &letter: row) {
      letters.append(letter);
    }
  }
  for(const auto &letter: letters) {
    QString svgFilename = mainSettings->optotypesDir + "/" + optotype + "/" + letter + ".svg";
    if(QFileInfo::exists(svgFilename)) {
      QGraphicsSvgItem *svgLetter = new QGraphicsSvgItem(svgFilename);
      svgLetter->setX(curX);
      layer->addToGroup(svgLetter);
      curX += svgLetter->boundingRect().width() + spaceWidth;
    } else {
      QMessageBox::warning(nullptr, tr("File not found"), tr("File '") + svgFilename + tr("' not found. Please correct this."));
    }
  }

  rowPair.first = size;
  rowPair.second = layer;

  rows.append(rowPair);
}

void OptotypeChart::updateAll()
{
  QString sizeStr = "";
  for(int a = 0; a < rows.length(); ++a) {
    if(a == currentRowIdx) {
      sizeStr = rows.at(a).first;
      double decimalSize = sizeStr.toDouble();
      double arcMinScaled = ((mainSettings->pxPerArcMin / ((10.0 * decimalSize) * 100.0)) * 100.0) * mainSettings->distanceFactor;
      double spaceWidthScaled = (spaceWidth / 100.0) * arcMinScaled;
      
      // Scale row to current decimalSize and patient distance
      rows.at(a).second->setScale((mainSettings->pxPerArcMin / ((10.0 * decimalSize) * 100.0)) * mainSettings->distanceFactor);

      // Calculate skew
      double currentSkew = ((rows.at(a).second->mapRectToScene(rows.at(a).second->boundingRect()).width() + spaceWidthScaled) / rows.at(a).second->childItems().length()) * skew;

      // Always use mapRectToScene to make sure current scaling is taken into account
      rows.at(a).second->setX((mainSettings->width / 2) - (rows.at(a).second->mapRectToScene(rows.at(a).second->boundingRect()).width() / 2) + currentSkew);
      rows.at(a).second->setY((mainSettings->height / 2) - (rows.at(a).second->mapRectToScene(rows.at(a).second->boundingRect()).height() / 2));

      rows.at(a).second->show();

      // Show / hide optotypes depending on whether they are cut off at screen edges
      // or if single is set
      for(auto *letter: rows.at(a).second->childItems()) {
        if(mainSettings->single) {
          if(rows.at(a).second->mapToScene(letter->pos()).x() + (rows.at(a).second->mapRectToScene(letter->boundingRect()).width() / 2.0) >= mainSettings->width / 2.0 - 2 &&
             rows.at(a).second->mapToScene(letter->pos()).x() + (rows.at(a).second->mapRectToScene(letter->boundingRect()).width() / 2.0) <= mainSettings->width / 2.0 + 2) {
            letter->show();
          } else {
            letter->hide();
          }
        } else {
          if(rows.at(a).second->mapToScene(letter->pos()).x() < 0 ||
             rows.at(a).second->mapToScene(letter->pos()).x() + rows.at(a).second->mapRectToScene(letter->boundingRect()).width() > mainSettings->width) {
            letter->hide();
          } else {
            letter->show();
          }
        }
      }

      int firstVisible = -1;
      int lastVisible = -1;
      for(int b = 0; b < rows.at(a).second->childItems().length(); ++b) {
        if(rows.at(a).second->childItems().at(b)->isVisible() && firstVisible == -1) {
          firstVisible = b;
        }
        if(rows.at(a).second->childItems().at(b)->isVisible()) {
          lastVisible = b;
        }
      }

      // Enable / disable crowding
      if(mainSettings->crowding) {
        QPen crowdingPen;
        crowdingPen.setWidth(arcMinScaled);
        crowdingPen.setColor(QColor(0, 0, 0));
        crowdingPen.setCapStyle(Qt::FlatCap);
        crowdingPen.setJoinStyle(Qt::MiterJoin);
        crowdRect->setPen(crowdingPen);
        crowdRect->setRect(rows.at(a).second->mapToScene(rows.at(a).second->childItems().at(firstVisible)->pos()).x() - ((crowdingSpan + 0.5) * arcMinScaled),
                           rows.at(a).second->mapToScene(rows.at(a).second->childItems().at(firstVisible)->pos()).y() - ((crowdingSpan + 0.5) * arcMinScaled),
                           rows.at(a).second->mapToScene(rows.at(a).second->childItems().at(lastVisible)->pos()).x() - rows.at(a).second->mapToScene(rows.at(a).second->childItems().at(firstVisible)->pos()).x() + rows.at(a).second->mapRectToScene(rows.at(a).second->childItems().at(lastVisible)->boundingRect()).width() + (((crowdingSpan + 0.5) * 2.0) * arcMinScaled),
                           rows.at(a).second->mapRectToScene(rows.at(a).second->boundingRect()).height() + (((crowdingSpan + 0.5) * 2.0) * arcMinScaled));
        crowdRect->show();
      } else {
        crowdRect->hide();
      }
    } else {
      rows.at(a).second->hide();
    }
  }

  QFont font;
  font.setFamily("Arial");
  font.setPixelSize(mainSettings->pxPerArcMin * mainSettings->distanceFactor * 1.5);

  // Add the letter size visual help
  sizeItem->setText(sizeStr);
  sizeItem->setFont(font);
  sizeItem->setX(10);
  sizeItem->setY(mainSettings->height - sizeItem->boundingRect().height() - 10);

  // Re-add title
  titleItem->setFont(font);
  titleItem->setX(mainSettings->width - titleItem->boundingRect().width() - 10);
  titleItem->setY(mainSettings->height - titleItem->boundingRect().height() - 10);

  // Update entire screen area
  update(0, 0, mainSettings->width, mainSettings->height);
}

void OptotypeChart::setStartSize(const QString &startSize)
{
  this->startSize = startSize;
}

void OptotypeChart::resetSize()
{
  printf("Resetting size!\n");
  setSize(startSize);
}

void OptotypeChart::setSize(const QString &sizeStr)
{
  for(int a = 0; a < rows.length(); ++a) {
    if(rows.at(a).first == sizeStr) {
      currentRowIdx = a;
      break;
    }
  }
  updateAll();
}

QString OptotypeChart::getSize()
{
  return rows.at(currentRowIdx).first;
}
