/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lettersize.cpp
 *
 *  Sun Apr 15 20:00:00 UTC+1 2018
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

#include "lettersize.h"

#include <stdio.h>
#include <math.h>

#include <QFile>

LetterSize::LetterSize(MainSettings *mainSettings, const QList<QString> sizes, const QString startSize)
{
  this->mainSettings = mainSettings;
  this->sizes = sizes;
  sizeIdx = 0;
  setSize(startSize);
}

LetterSize::~LetterSize()
{
}

bool LetterSize::setSize(QString size)
{
  for(int a = 0; a < this->sizes.length(); ++a) {
    if(sizes.at(a) == size) {
      sizeIdx = a;
      emit refresh();
      return true;
      break;
    }
  }
  return false;
}

void LetterSize::increaseSize()
{
  if(sizeIdx > 0)
    sizeIdx--;
  emit refresh();
}

void LetterSize::decreaseSize()
{
  if(sizeIdx < sizes.length() - 1)
    sizeIdx++;
  emit refresh();
}

double LetterSize::getDecimalSize()
{
  return sizes.at(sizeIdx).toDouble();
}

QString LetterSize::getSizeStr()
{
  return sizes.at(sizeIdx);
}

int LetterSize::getPixelSize()
{
  double decimalSize = sizes.at(sizeIdx).toDouble();
  // The fiveArcMinutes value is the pixel height of 1 letter at the 6 meter standard distance
  // https://en.wikipedia.org/wiki/LogMAR_chart
  return round(((mainSettings->pxPerArcMin * 5.0) / (10.0 * decimalSize)) *
               mainSettings->distanceFactor);
}
