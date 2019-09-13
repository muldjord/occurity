/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lettermode.cpp
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

#include "lettermode.h"

#include <stdio.h>
#include <math.h>
#include <QKeyEvent>

LetterMode::LetterMode(Settings *mainSettings, QString chartFile, QObject *parent) :
  QGraphicsScene(0, 0, mainSettings->width, mainSettings->height, parent)
{
  this->mainSettings = mainSettings;

  size = new LetterSize(mainSettings);
  
  loadRowStrings(chartFile);
  addRow();
  
  sizeItem = new QGraphicsSimpleTextItem("0.0");
  QFont font;
  font.setFamily("Arial");
  font.setPixelSize(mainSettings->fiveArcMinutes * mainSettings->distanceFactor * 2.0);
  sizeItem->setFont(font);

  titleItem = new QGraphicsSimpleTextItem(rowStrings.first());
  font.setPixelSize(mainSettings->fiveArcMinutes * mainSettings->distanceFactor * 2.0);
  titleItem->setFont(font);

  updateAll();
}

LetterMode::~LetterMode()
{
}

void LetterMode::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Up) {
    increaseSize();
  } else if(event->key() == Qt::Key_Down) {
    decreaseSize();
  } else if(event->key() == Qt::Key_Left) {
    skew++;
  } else if(event->key() == Qt::Key_Right) {
    skew--;
  } else if(event->key() == Qt::Key_Period) {
    addRow();
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
      LetterRow *tmpRow = new LetterRow(rowStrings);
      tmpRow->setLetters(perRow, size->getSizeStr());
      tmpRow->setPixelSize(size->getPixelSize());
      letterRows.append(tmpRow);
    }
  } else if(event->key() == Qt::Key_R) {
    shuffleAllRows();
  }
  updateAll();
}

void LetterMode::loadRowStrings(QString filename)
{
  QFile input(filename);
  if(input.open(QIODevice::ReadOnly)) {
    while(!input.atEnd()) {
      rowStrings.append(input.readLine().simplified());
    }
    input.close();
  }
}

void LetterMode::refreshAllRows()
{
  int rows = letterRows.length();
  qDeleteAll(letterRows);
  letterRows.clear();
  for(int a = 0; a < rows; ++a) {
    addRow();
  }
}

void LetterMode::shuffleAllRows()
{
  foreach(LetterRow *letterRow, letterRows) {
    letterRow->shuffleLetters();
    letterRow->setPixelSize(size->getPixelSize());
  }
}

void LetterMode::addRow()
{
  if(letterRows.length() < 2) {
    LetterRow *tmpRow = new LetterRow(rowStrings);
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
    tmpRow->setPixelSize(size->getPixelSize());
    letterRows.append(tmpRow);
  }
}

void LetterMode::removeRow()
{
  if(letterRows.length() > 1) {
    // FIXME: Delete pointer also?
    delete letterRows.last();
    letterRows.removeLast();
  }
}

void LetterMode::increaseSize()
{
  size->increaseSize();
  printf("New size: %s\n", size->getSizeStr().toStdString().c_str());
  refreshAllRows();
  /*
  foreach(LetterRow *letterRow, letterRows) {
    letterRow->setPixelSize(size->getPixelSize());
  }
  */
}

void LetterMode::decreaseSize()
{
  size->decreaseSize();
  refreshAllRows();
  /*
  foreach(LetterRow *letterRow, letterRows) {
    letterRow->setPixelSize(size->getPixelSize());
  }
  */
}

void LetterMode::updateAll()
{
  foreach(QGraphicsItem *item, items()) {
    removeItem(item);
  }

  int rows = letterRows.size();
  int ySpacing = mainSettings->height / (rows + 1);
  int y = 0;

  foreach(LetterRow *letterRow, letterRows) {
    y += ySpacing;
    int letters = letterRow->letters.length();
    double xSpacing = size->getPixelSize() * 2;
    double x = (mainSettings->width / 2) - (size->getPixelSize() * (letters - 1));
    for(int a = 0; a < letters; ++a) {
      int currentX = x - (size->getPixelSize() / 2) + (skew * xSpacing);
      if(currentX >= 1 && currentX + size->getPixelSize() <= mainSettings->width) {
        addItem(letterRow->letters.at(a));
        letterRow->letters.at(a)->setX(currentX);
        letterRow->letters.at(a)->setY(y - (size->getPixelSize() / 2));
      }
      x += xSpacing;
    }
  }

  // Add the letter size visual help
  sizeItem->setText(size->getSizeStr());
  addItem(sizeItem);
  sizeItem->setX(10);
  sizeItem->setY(mainSettings->height - titleItem->boundingRect().height() - 10);

  // Re-add title
  addItem(titleItem);
  titleItem->setX(mainSettings->width - titleItem->boundingRect().width() - 10);
  titleItem->setY(mainSettings->height - titleItem->boundingRect().height() - 10);

  // Update entire screen area
  update(0, 0, mainSettings->width, mainSettings->height);
}
