/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lettersize.h
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
#ifndef __OCCURITY_LETTERSIZE_H__
#define __OCCURITY_LETTERSIZE_H__

#include "mainsettings.h"

#include <QObject>

class LetterSize : public QObject
{
  Q_OBJECT

public:
  LetterSize(MainSettings *mainSettings, const QList<QString> sizes, const QString startSize);
  ~LetterSize();
  void setSizeFromDecimal(double value);
  void setSizeFromLogMAR(double value);
  double getDecimalSize();
  double getLogMARSize();
  int getPixelSize();
  bool setSize(QString size);
  void increaseSize();
  void decreaseSize();
  QString getSizeStr();

signals:
  void refresh();

private:
  MainSettings *mainSettings;
  QList<QString> sizes;
  int sizeIdx = -1;

};
#endif/*__OCCURITY_LETTERSIZE_H__*/
