/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            letterrow.cpp
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

#include "letterrow.h"

#include <QRandomGenerator>

LetterRow::LetterRow(QList<QPair<QString, QString> > *rowStrings, QString family)
{
  this->rowStrings = rowStrings;
  this->family = family;
}

LetterRow::~LetterRow()
{
  qDeleteAll(letters);
}

QString LetterRow::setLetters(int chars, QString decimalStr)
{
  currentRow.clear();

  if(rowStrings->isEmpty()) {
    return QString("No rows!");
  }

  QList<QString> fittingRows;
  for(int a = 0; a < rowStrings->length(); ++a) {
    QPair<QString, QString> row = rowStrings->at(a);
    if(row.first == decimalStr) {
      fittingRows.append(row.second);
    }
  }

  currentRow = fittingRows.at(QRandomGenerator::global()->bounded(fittingRows.size()));
  qDeleteAll(letters);
  letters.clear();

  for(int a = 0; a < chars; ++a) {
    letters.append(new Letter(currentRow.at(a), family));
  }

  return currentRow;
}

QString LetterRow::getLetters()
{
  QString currentLetters = "";
  foreach(Letter *letter, letters) {
    currentLetters.append(letter->text());
  }
  return currentLetters;
}

void LetterRow::shuffleLetters(bool lockCurrent)
{
  QString tmpRow = getLetters();
  int chars = tmpRow.length();

  if(!lockCurrent) {
    tmpRow = currentRow;
  }

  qDeleteAll(letters);
  letters.clear();

  while(chars--) {
    int chosenLetter = QRandomGenerator::global()->bounded(tmpRow.length());
    letters.append(new Letter(tmpRow.at(chosenLetter), family));
    tmpRow.remove(chosenLetter, 1);
  }
}

void LetterRow::setPixelSize(int size)
{
  foreach(Letter *letter, letters) {
    letter->setPixelSize(size);
  }
}
