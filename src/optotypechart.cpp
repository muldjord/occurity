/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            optotypechart.cpp
 *
 *  Tue Jan 11 14:32:00 UTC+1 2022
 *  Copyright 2022 Lars Bisballe
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

#include "optotypechart.h"
#include "optosymbol.h"

#include <stdio.h>
#include <math.h>

#include <QKeyEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QLabel>
#include <QMovie>

OptotypeChart::OptotypeChart(MainSettings &mainSettings, QObject *parent) :
  AbstractChart(mainSettings, parent)
{
}

OptotypeChart::~OptotypeChart()
{
  for(auto *row: rows) {
    delete row;
  }
}

void OptotypeChart::init()
{
  if(!animation.isEmpty() && QFileInfo::exists(animation)) {
    QFileInfo animInfo(animation);
    if(animInfo.suffix() == "gif") {
      QLabel *gifAnim = new QLabel();
      QMovie *movie = new QMovie(animation);
      gifAnim->setMovie(movie);
      movie->start();
      animItem = addWidget(gifAnim);
      animItem->setZValue(-1);
      animItem->setPos(mainSettings.width / 2.0 - animItem->boundingRect().width() / 2.0,
                       mainSettings.height / 4.0 - animItem->boundingRect().height() / 2.0);
      animItem->hide();
    }
  }

  crowdRect = addRect(0, 0, 10, 10);

  sizeItem = new QGraphicsSimpleTextItem("0.0");
  copyrightItem = new QGraphicsSimpleTextItem("");
  QFont font;
  font.setFamily("Arial");
  font.setPixelSize((mainSettings.pxPerArcMin * 5.0) * mainSettings.distanceFactor * 0.2);
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
  copyrightItem->setPos((mainSettings.width / 2.0) - (copyrightItem->boundingRect().width() / 2.0), mainSettings.height - 35);

  setSize(startSize);
}

void OptotypeChart::makeIdle()
{
  if(animItem != nullptr) {
    animItem->hide();
  }
}

void OptotypeChart::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Up ||
     event->key() == Qt::Key_Down ||
     event->key() == Qt::Key_PageUp ||
     event->key() == Qt::Key_PageDown) {
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
    } else if(event->key() == Qt::Key_PageUp) {
      currentRowIdx -= mainSettings.rowSkipDelta;
      if(currentRowIdx < 0) {
        currentRowIdx = 0;
      }
    } else if(event->key() == Qt::Key_PageDown) {
      currentRowIdx += mainSettings.rowSkipDelta;
      if(currentRowIdx > rows.length() - 1) {
        currentRowIdx = rows.length() - 1;
      }
    }
    positionReset();
  } else if(event->key() == Qt::Key_Left) {
    if(skew > - (rows.at(currentRowIdx)->getLetters()->childItems().length() / 2)) {
      skew--;
    }
  } else if(event->key() == Qt::Key_Right) {
    if(skew < rows.at(currentRowIdx)->getLetters()->childItems().length() / 2) {
      skew++;
    }
  } else if(event->key() == Qt::Key_C) {
    mainSettings.crowding = !mainSettings.crowding;
  } else if(event->key() == Qt::Key_M) {
    mainSettings.single = !mainSettings.single;
  } else if(event->key() == Qt::Key_R) {
    QList<QPointF> availableCoords;
    for(const auto &child: rows.at(currentRowIdx)->getLetters()->childItems()) {
      availableCoords.append(child->pos());
    }
    for(const auto &child: rows.at(currentRowIdx)->getLetters()->childItems()) {
      int chosen = QRandomGenerator::global()->bounded(availableCoords.length());
      child->setPos(availableCoords.at(chosen));
      availableCoords.removeAt(chosen);
    }
  } else if(event->key() == Qt::Key_A && animItem != nullptr) {
    if(animItem->isVisible()) {
      animItem->hide();
    } else {
      animItem->show();
    }
  }

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

void OptotypeChart::addRow(const QString &size, const QString &caption, const QString &rowChars)
{
  /*
    Don't do any resizing / scaling here. It has to be done in updateAll(), otherwise it
    won't be re-scaled when preferences are changed.
    Here we simply load all SVG's into QGraphicsSvgItems, move them into place and add them
    to QGraphicsItemGroups.
  */
  OptoSymbol *svgSpace = new OptoSymbol(mainSettings.optotypesFolder + "/" + optotype + "/_.svg",
                                        fadeTimings,
                                        fadeLevels);
  spaceWidth = svgSpace->boundingRect().width();

  QGraphicsItemGroup *layer = new QGraphicsItemGroup;
  addItem(layer);

  int curX = 0;
  QList<QString> letters;
  if(rowChars.contains(";")) {
    for(const auto &letter: rowChars.split(";")) {
      letters.append(letter);
    }
  } else {
    for(const auto &letter: rowChars) {
      letters.append(letter);
    }
  }
  for(const auto &letter: letters) {
    QString svgFilename = mainSettings.optotypesFolder + "/" + optotype + "/" + letter + ".svg";
    if(QFileInfo::exists(svgFilename)) {
      OptoSymbol *svgLetter = new OptoSymbol(svgFilename, fadeTimings, fadeLevels);
      svgLetter->setX(curX);
      svgLetter->setData(0, svgLetter->pos());
      layer->addToGroup(svgLetter);
      curX += svgLetter->boundingRect().width() + spaceWidth;
      svgLetter->hide();
    } else {
      QMessageBox::warning(nullptr, tr("File not found"), tr("File '") + svgFilename + tr("' not found. Please correct this."));
    }
  }

  OptotypeRow *optotypeRow = new OptotypeRow(size, caption, layer);

  rows.append(optotypeRow);
}

void OptotypeChart::updateAll()
{
  QString rowCaption = "";
  for(int a = 0; a < rows.length(); ++a) {
    if(a == currentRowIdx) {
      if(mainSettings.useRowCaptions && !rows.at(a)->getCaption().isEmpty()) {
        rowCaption = rows.at(a)->getCaption();
      } else {
        rowCaption = rows.at(a)->getSize();
      }
      double decimalSize = rows.at(a)->getSize().toDouble();
      double arcMinScaled = ((mainSettings.pxPerArcMin / ((10.0 * decimalSize) * 100.0)) * 100.0) * mainSettings.distanceFactor;
      double spaceWidthScaled = (spaceWidth / 100.0) * arcMinScaled;

      // Scale row to current decimalSize and patient distance
      rows.at(a)->getLetters()->setScale((mainSettings.pxPerArcMin / ((10.0 * decimalSize) * 100.0)) * mainSettings.distanceFactor);

      // Calculate skew
      double currentSkew = ((rows.at(a)->getLetters()->mapRectToScene(rows.at(a)->getLetters()->boundingRect()).width() + spaceWidthScaled) / rows.at(a)->getLetters()->childItems().length()) * skew;

      // Always use mapRectToScene to make sure current scaling is taken into account
      rows.at(a)->getLetters()->setX((mainSettings.width / 2) - (rows.at(a)->getLetters()->mapRectToScene(rows.at(a)->getLetters()->boundingRect()).width() / 2) + currentSkew);
      rows.at(a)->getLetters()->setY((mainSettings.height / 2) - (rows.at(a)->getLetters()->mapRectToScene(rows.at(a)->getLetters()->boundingRect()).height() / 2));

      for(QGraphicsItem *graphicsItem: rows.at(a)->getLetters()->childItems()) {
        static_cast<OptoSymbol *>(graphicsItem)->fadeIn();
        //rows.at(a)->getLetters()->show();
      }

      // Show / hide optotypes depending on whether they are cut off at screen edges
      // or if single is set
      for(QGraphicsItem *graphicsItem: rows.at(a)->getLetters()->childItems()) {
        OptoSymbol *optoSymbol = static_cast<OptoSymbol *>(graphicsItem);
        if(mainSettings.single) {
          if(rows.at(a)->getLetters()->mapToScene(optoSymbol->pos()).x() + (rows.at(a)->getLetters()->mapRectToScene(optoSymbol->boundingRect()).width() / 2.0) >= mainSettings.width / 2.0 - 2 &&
             rows.at(a)->getLetters()->mapToScene(optoSymbol->pos()).x() + (rows.at(a)->getLetters()->mapRectToScene(optoSymbol->boundingRect()).width() / 2.0) <= mainSettings.width / 2.0 + 2) {
            optoSymbol->fadeIn();
          } else {
            optoSymbol->fadeInOut();
          }
        } else {
          if(rows.at(a)->getLetters()->mapToScene(optoSymbol->pos()).x() < 0 ||
             rows.at(a)->getLetters()->mapToScene(optoSymbol->pos()).x() + rows.at(a)->getLetters()->mapRectToScene(optoSymbol->boundingRect()).width() > mainSettings.width) {
            optoSymbol->fadeInOut();
          } else {
            optoSymbol->fadeIn();
          }
        }
      }

      // Enable / disable crowding
      if(mainSettings.crowding) {
        QPen crowdingPen;
        crowdingPen.setWidth(arcMinScaled);
        crowdingPen.setColor(QColor(0, 0, 0));
        crowdingPen.setCapStyle(Qt::FlatCap);
        crowdingPen.setJoinStyle(Qt::MiterJoin);
        crowdRect->setPen(crowdingPen);

        int leftMost = 0;
        double leftX = 100000000;
        int rightMost = 0;
        double rightX = 0;
        for(int b = 0; b < rows.at(a)->getLetters()->childItems().length(); ++b) {
          if(static_cast<OptoSymbol *>(rows.at(a)->getLetters()->childItems().at(b))->isHidden()) {
            continue;
          }
          if(rows.at(a)->getLetters()->childItems().at(b)->pos().x() <= leftX) {
            leftMost = b;
            leftX = rows.at(a)->getLetters()->childItems().at(b)->pos().x();
          }
          if(rows.at(a)->getLetters()->childItems().at(b)->pos().x() >= rightX) {
            rightMost = b;
            rightX = rows.at(a)->getLetters()->childItems().at(b)->pos().x();
          }
        }

        crowdRect->setRect(rows.at(a)->getLetters()->mapToScene(rows.at(a)->getLetters()->childItems().at(leftMost)->pos()).x() - ((crowdingSpan + 0.5) * arcMinScaled),
                           rows.at(a)->getLetters()->mapToScene(rows.at(a)->getLetters()->childItems().at(leftMost)->pos()).y() - ((crowdingSpan + 0.5) * arcMinScaled),
                           rows.at(a)->getLetters()->mapToScene(rows.at(a)->getLetters()->childItems().at(rightMost)->pos()).x() - rows.at(a)->getLetters()->mapToScene(rows.at(a)->getLetters()->childItems().at(leftMost)->pos()).x() + rows.at(a)->getLetters()->mapRectToScene(rows.at(a)->getLetters()->childItems().at(rightMost)->boundingRect()).width() + (((crowdingSpan + 0.5) * 2.0) * arcMinScaled),
                           rows.at(a)->getLetters()->mapRectToScene(rows.at(a)->getLetters()->boundingRect()).height() + (((crowdingSpan + 0.5) * 2.0) * arcMinScaled));
        crowdRect->show();
      } else {
        crowdRect->hide();
      }
    } else {
      for(QGraphicsItem *graphicsItem: rows.at(a)->getLetters()->childItems()) {
        static_cast<OptoSymbol *>(graphicsItem)->fadeOut();
        //rows.at(a)->getLetters()->hide();
      }
    }
  }

  QFont font;
  font.setFamily("Arial");
  font.setPixelSize(mainSettings.pxPerArcMin * mainSettings.distanceFactor * 1.5);

  // Add the optoSymbol size visual help
  sizeItem->setText(rowCaption);
  sizeItem->setFont(font);
  sizeItem->setX(10);
  sizeItem->setY(mainSettings.height - sizeItem->boundingRect().height() - 10);

  // Re-add title
  titleItem->setFont(font);
  titleItem->setX(mainSettings.width - titleItem->boundingRect().width() - 10);
  titleItem->setY(mainSettings.height - titleItem->boundingRect().height() - 10);

  // Update entire screen area
  update(0, 0, mainSettings.width, mainSettings.height);
}

void OptotypeChart::setStartSize(const QString &startSize)
{
  this->startSize = startSize;
}

void OptotypeChart::setFadeTimings(const QString &fadeTimings)
{
  this->fadeTimings = fadeTimings;
}

void OptotypeChart::setFadeLevels(const QString &fadeLevels)
{
  this->fadeLevels = fadeLevels;
}

void OptotypeChart::resetAll()
{
  skew = 0;
  setSize(startSize);
}

void OptotypeChart::setSize(const QString &sizeStr)
{
  for(int a = 0; a < rows.length(); ++a) {
    if(rows.at(a)->getSize() == sizeStr) {
      currentRowIdx = a;
      break;
    }
  }
  positionReset();
  updateAll();
}

QString OptotypeChart::getSize()
{
  return rows.at(currentRowIdx)->getSize();
}

void OptotypeChart::positionReset()
{
  // Reset positions for all optotype symbols in row in case they've been randomized
  for(const auto &child: rows.at(currentRowIdx)->getLetters()->childItems()) {
    child->setPos(child->data(0).toPointF());
  }
}

void OptotypeChart::setAnimation(const QString &animation)
{
  this->animation = animation;
}

void OptotypeChart::hideFromList()
{
  for(auto *item: hideList) {
    item->hide();
  }
  hideList.clear();
}
