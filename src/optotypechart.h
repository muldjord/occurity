/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            optotypechart.h
 *
 *  Tue Jan 11 14:32:00 UTC+1 2022
 *  Copyright 2022 Lars Bisballe
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
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QGraphicsProxyWidget>

class OptotypeChart : public AbstractChart
{
  Q_OBJECT

public:
  OptotypeChart(MainSettings &mainSettings, QObject *parent);
  ~OptotypeChart();
  void init() override;
  void makeIdle() override;
  void setOptotype(const QString &optotype);
  QString getOptotype();
  void setCrowdingSpan(const double &crowdingSpan);
  double getCrowdingSpan();
  void setAnimation(const QString &animation);
  void setSize(const QString &sizeStr) override;
  QString getSize() override;
  void addRow(const QString &size, const QString &row);
  void setStartSize(const QString &startSize);

public slots:
  void updateAll() override;
  void resetAll() override;

protected:
  void keyPressEvent(QKeyEvent *event) override;
  
private:
  QString startSize = "0.16";
  QGraphicsProxyWidget *animItem = nullptr;

  QString optotype = "";
  double crowdingSpan = 2.5;
  QString animation = "";
  
  int skew = 0;
  double spaceWidth = 0.0;

  void addRow();
  void positionReset();

  QList<QPair<QString, QGraphicsItemGroup *> > rows;
  int currentRowIdx = 0;
  QGraphicsSimpleTextItem *sizeItem;
  QGraphicsSimpleTextItem *copyrightItem;
  QGraphicsRectItem *crowdRect;

};
#endif/*__VISUTEST_OPTOTYPECHART_H__*/
