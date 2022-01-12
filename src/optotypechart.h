/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            optotypechart.h
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
#ifndef __VISUTEST_OPTOTYPECHART_H__
#define __VISUTEST_OPTOTYPECHART_H__

#include "letterrow.h"
#include "lettersize.h"
#include "mainsettings.h"
#include "abstractchart.h"

#include <QTimer>

class OptotypeChart : public AbstractChart
{
  Q_OBJECT
    
public:
  OptotypeChart(MainSettings *mainSettings, QObject *parent);
  ~OptotypeChart();
  void init() override;
  void setOptotype(QString family) override;
  QString getOptotype() override;
  void setSizeLocked(const bool &sizeLocked) override;
  bool isSizeLocked() override;
  void setSize(const QString &sizeStr) override;
  QString getSize() override;
                                                       
public slots:
  void updateAll() override;

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void addRowString(QString size, QString row) override;
  void setRowSizes(QList<QString> rowSizeStrings) override;
  void setStartSize(const QString startSize) override;

private slots:
  void refreshAllRows();
  void resetSize();

private:
  QString family = "";
  bool sizeLocked = false;
  bool crowding = false;
  QList<QPair<QString, QString> > rowStrings;
  QList<QString> rowSizes;
  
  LetterSize *size;

  int perRow = 5;
  int skew = 0;

  void shuffleRow(LetterRow *row);
  void shuffleAllRows();
  void addRow();
  void removeRow();
  void setLogMARSize(double value);
  //void loadRowStrings(QString filename);
  QList<LetterRow *> letterRows;
  QGraphicsSimpleTextItem *sizeItem;

  QTimer sizeResetTimer;
  
};
#endif/*__VISUTEST_OPTOTYPECHART_H__*/