/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            fontchart.cpp
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

#include "fontchart.h"

#include <stdio.h>
#include <math.h>
#include <QKeyEvent>

FontChart::FontChart(MainSettings *mainSettings, QObject *parent) :
  AbstractChart(mainSettings, parent)
{
  sizeResetTimer.setInterval(mainSettings->sizeResetTime);
  sizeResetTimer.setSingleShot(true);
  connect(&sizeResetTimer, &QTimer::timeout, this, &FontChart::resetSize);
}

FontChart::~FontChart()
{
}

void FontChart::init()
{
  size = new LetterSize(mainSettings, rowSizes, startSize);
  connect(size, &LetterSize::refresh, this, &FontChart::refreshAllRows);

  sizeItem = new QGraphicsSimpleTextItem("0.0");
  QFont font;
  font.setFamily("Arial");
  font.setPixelSize(mainSettings->fiveArcMinutes * mainSettings->distanceFactor * 2.0);
  sizeItem->setFont(font);

  titleItem->setText(objectName());

  addRow();

  updateAll();
}

void FontChart::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Up) {
    size->increaseSize();
    if(letterRows.length() > 1) {
      shuffleRow(letterRows.last());
    }
  } else if(event->key() == Qt::Key_Down) {
    size->decreaseSize();
    if(letterRows.length() > 1) {
      shuffleRow(letterRows.last());
    }
  } else if(event->key() == Qt::Key_Left) {
    if(skew < letterRows.first()->letters.length() / 2)
      skew++;
  } else if(event->key() == Qt::Key_Right) {
    if(skew > - letterRows.first()->letters.length() / 2)
      skew--;
  } else if(event->key() == Qt::Key_Period) {
    addRow();
    if(letterRows.length() > 1) {
      shuffleRow(letterRows.last());
    }
  } else if(event->key() == Qt::Key_C) {
    crowding = !crowding;
  } else if(event->key() == Qt::Key_Comma) {
    removeRow();
  } else if(event->key() == Qt::Key_M) {
    if(perRow == 5) {
      perRow = 1;
    } else {
      perRow = 5;
    }
    skew = 0;
    int rows = letterRows.length();
    letterRows.clear();
    for(int a = 0; a < rows; ++a) {
      LetterRow *tmpRow = new LetterRow(&rowStrings, family);
      tmpRow->setLetters(perRow, size->getSizeStr());
      letterRows.append(tmpRow);
    }
    if(letterRows.length() > 1) {
      shuffleRow(letterRows.last());
    }
  } else if(event->key() == Qt::Key_R) {
    shuffleAllRows();
  }

  // Update reset interval in case it has changed
  sizeResetTimer.setInterval(mainSettings->sizeResetTime);
  sizeResetTimer.start();

  updateAll();
}

void FontChart::refreshAllRows()
{
  int rows = letterRows.length();
  qDeleteAll(letterRows);
  letterRows.clear();
  for(int a = 0; a < rows; ++a) {
    addRow();
  }
}

void FontChart::shuffleRow(LetterRow *row)
{
  row->shuffleLetters();
}

void FontChart::shuffleAllRows()
{
  for(const auto letterRow: letterRows) {
    letterRow->shuffleLetters();
  }
}

void FontChart::addRow()
{
  if(letterRows.length() < 2) {
    LetterRow *tmpRow = new LetterRow(&rowStrings, family);
    /*
    bool rowExists;
    do {
      rowExists = false;
      QString currentLetters = tmpRow->setLetters(perRow, size->getSizeStr());
      for(int a = 0; a < letterRows.length(); ++a) {
        if(currentLetters == letterRows.at(a)->getLetters()) {
          rowExists = true;
          break;
        }
      }
    } while(rowExists);
    */
    tmpRow->setLetters(perRow, size->getSizeStr());
    letterRows.append(tmpRow);
  }
}

void FontChart::removeRow()
{
  if(letterRows.length() > 1) {
    // FIXME: Delete pointer also?
    delete letterRows.last();
    letterRows.removeLast();
  }
}

void FontChart::setOptotype(QString family)
{
  this->family = family;
}

QString FontChart::getOptotype()
{
  return family;
}

void FontChart::setSizeLocked(const bool &sizeLocked)
{
  this->sizeLocked = sizeLocked;
}

bool FontChart::isSizeLocked()
{
  return sizeLocked;
}

void FontChart::addRowString(QString size, QString row)
{
  rowStrings.append(QPair<QString, QString> (size, row));
}

void FontChart::setRowSizes(QList<QString> rowSizes)
{
  this->rowSizes = rowSizes;
}

void FontChart::updateAll()
{
  for(const auto item: items()) {
    removeItem(item);
  }

  int rows = letterRows.size();
  int ySpacing = mainSettings->height / (rows + 1);
  int y = 0;

  Letter spaceChar(" ", family);
  spaceChar.setPixelSize(size->getPixelSize());
  double spaceWidth = spaceChar.boundingRect().width();
  double spaceHeight = spaceChar.boundingRect().height();

  QPen crowdingPen;
  crowdingPen.setWidth(spaceWidth / 5.0);
  crowdingPen.setColor(QColor(0, 0, 0));
  crowdingPen.setCapStyle(Qt::FlatCap);
  crowdingPen.setJoinStyle(Qt::MiterJoin);

  double crowdingSpan = (spaceHeight / 5.0) * 1.5;

  for(const auto letterRow: letterRows) {
    QPoint upperLeft(mainSettings->width, mainSettings->height); // Is adjusted later
    QPoint lowerRight(0, 0); // Is adjusted later

    letterRow->setPixelSize(spaceWidth);
    int letters = letterRow->letters.length();
    int centerIdx = letters / 2;
    QList<int> xSpaces;
    for(const auto letter: letterRow->letters) {
      xSpaces.append(letter->boundingRect().width());
    }

    int currentX = mainSettings->width / 2;
    for(int a = 0; a < centerIdx + skew; ++a) {
      currentX -= xSpaces.at(a) + spaceWidth;
    }
    currentX -= xSpaces.at(centerIdx + skew) / 2;
    y += ySpacing;
    for(int a = 0; a < letters; ++a) {
      if(currentX > 0 && currentX + xSpaces.at(a) < mainSettings->width) {
        addItem(letterRow->letters.at(a));
        letterRow->letters.at(a)->setX(currentX);
        letterRow->letters.at(a)->setY(y - (spaceHeight / 2));
        // Prepare coordinates for crowding
        QRectF letterRect = letterRow->letters.at(a)->boundingRect();
        if(upperLeft.x() > currentX - crowdingSpan) {
          upperLeft.setX(currentX - crowdingSpan);
        }
        upperLeft.setY(y - (letterRect.width() / 2.0) - crowdingSpan);
        if(lowerRight.x() < currentX + letterRect.width() + crowdingSpan) {
          lowerRight.setX(currentX + letterRect.width() + crowdingSpan);
        }
        lowerRight.setY(letterRect.width() + (spaceHeight / 5.0) * 3.0);
      }
      currentX += xSpaces.at(a) + spaceWidth;
    }
    if(crowding) {
      addRect(upperLeft.x(), upperLeft.y(), lowerRight.x() - upperLeft.x(), lowerRight.y(), crowdingPen);
    }
  }

  QFont font;
  font.setFamily("Arial");
  font.setPixelSize(mainSettings->fiveArcMinutes * mainSettings->distanceFactor * 3.0);

  // Add the letter size visual help
  sizeItem->setText(size->getSizeStr());
  sizeItem->setFont(font);
  addItem(sizeItem);
  sizeItem->setX(10);
  sizeItem->setY(mainSettings->height - sizeItem->boundingRect().height() - 10);

  // Re-add title
  titleItem->setFont(font);
  addItem(titleItem);
  titleItem->setX(mainSettings->width - titleItem->boundingRect().width() - 10);
  titleItem->setY(mainSettings->height - titleItem->boundingRect().height() - 10);

  // Update entire screen area
  update(0, 0, mainSettings->width, mainSettings->height);
}

void FontChart::setStartSize(const QString startSize)
{
  this->startSize = startSize;
}

void FontChart::resetSize()
{
  printf("Resetting size!\n");
  size->setSize(startSize);
  updateAll();
}

void FontChart::setSize(const QString &sizeStr)
{
  size->setSize(sizeStr);
  updateAll();
}

QString FontChart::getSize()
{
  return size->getSizeStr();
}
