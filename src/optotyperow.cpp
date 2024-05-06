/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            optotyperow.cpp
 *
 *  Mon May 6 10:34:00 UTC+1 2024
 *  Copyright 2024 Lars Bisballe
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

#include "optotyperow.h"

#include <stdio.h>

OptotypeRow::OptotypeRow() : QObject()
{
}

OptotypeRow::OptotypeRow(const OptotypeRow &data) : QObject()
{
  this->size = data.size;
  this->caption = data.caption;
  this->letters = data.letters;
}

void OptotypeRow::operator=(const OptotypeRow &data)
{
  this->size = data.size;
  this->caption = data.caption;
  this->letters = data.letters;
}

OptotypeRow::OptotypeRow(const QString &size, const QString &caption, QGraphicsItemGroup *letters)
{
  this->size = size;
  this->caption = caption;
  this->letters = letters;
}

OptotypeRow::~OptotypeRow()
{
  delete letters;
}

QString OptotypeRow::getSize() const
{
  return size;
}

QString OptotypeRow::getCaption() const
{
  return caption;
}

QGraphicsItemGroup *OptotypeRow::getLetters() const
{
  return letters;
}
