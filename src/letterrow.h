/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            letterrow.h
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
#ifndef __OCCURITY_LETTERROW_H__
#define __OCCURITY_LETTERROW_H__

#include "letter.h"

class LetterRow
{
public:
  LetterRow(QList<QPair<QString, QString> > *rowStrings, QString family);
  ~LetterRow();
  QString setLetters(int chars, QString decimalStr);
  QString getLetters();
  void shuffleLetters(bool lockCurrent = false);
  void setPixelSize(int size);
  QList<Letter *> letters;
  //void addLetter(QChar letter);

private:
  QString family;
  QString currentRow;
  QList<QPair<QString, QString> > *rowStrings;
};
#endif/*__OCCURITY_LETTERROW_H__*/
