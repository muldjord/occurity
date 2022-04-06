/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            abstractchart.h
 *
 *  Mon Sep 3 12:00:00 UTC+1 2018
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
#ifndef __OCCURITY_ABSTRACTCHART_H__
#define __OCCURITY_ABSTRACTCHART_H__

#include "letterrow.h"
#include "lettersize.h"
#include "mainsettings.h"

#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsSimpleTextItem>

constexpr int OPTOTYPECHART = 0;
constexpr int SVGCHART = 1;

class AbstractChart : public QGraphicsScene
{
  Q_OBJECT

public:
  AbstractChart(MainSettings &mainSettings, QObject *parent);
  ~AbstractChart();
  virtual void init();
  virtual void makeIdle(){};
  // setName and getName has been replaced with the default objectName and setObjectName
  void setType(const QString &type);
  QString getType();
  void setNumKey(Qt::Key numKey);
  Qt::Key getNumKey();
  void setBgColor(QString color);
  MainSettings &mainSettings;
  void setSizeLocked(const bool &sizeLocked);
  bool isSizeLocked();
  virtual void setSize(const QString &sizeStr);
  virtual QString getSize();

protected:
  QGraphicsSimpleTextItem *titleItem;

public slots:
  virtual void updateAll(){};
  virtual void resetAll(){};

private:
  int type = -1;
  Qt::Key numKey = Qt::Key_0;
  QString sizeStr = "";
  bool sizeLocked = false;

};
#endif/*__OCCURITY_ABSTRACTCHART_H__*/
