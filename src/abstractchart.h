/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            abstractchart.h
 *
 *  Mon Sep 3 12:00:00 UTC+1 2018
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
#ifndef __VISUTEST_ABSTRACTCHART_H__
#define __VISUTEST_ABSTRACTCHART_H__

#include "letterrow.h"
#include "lettersize.h"
#include "mainsettings.h"

#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsSimpleTextItem>

constexpr int FONTCHART = 0;
constexpr int SVGCHART = 1;
constexpr int OPTOTYPECHART = 2;

class AbstractChart : public QGraphicsScene
{
  Q_OBJECT

public:
  AbstractChart(MainSettings *mainSettings, QObject *parent);
  ~AbstractChart();
  virtual void init();
  // setName and getName has been replaced with the default objectName and setObjectName
  void setType(const QString &type);
  QString getType();
  void setNumKey(Qt::Key numKey);
  Qt::Key getNumKey();
  void setBgColor(QString color);
  virtual void setSizeLocked(const bool &){};
  virtual bool isSizeLocked(){return false;};
  virtual void setSize(const QString &){};
  virtual QString getSize(){return QString();};
  virtual void setOptotype(const QString &){};
  virtual QString getOptotype();
  virtual void setCrowdingSpan(const double &){};
  virtual double getCrowdingSpan();
  virtual void setSource(const QString &){};
  virtual void addRow(const QString &, const QString &);
  virtual void setStartSize(const QString &){};
  virtual void setScaling(const QString &){};
  virtual bool addSvgLayer(const QString &){return false;};
  MainSettings *mainSettings;

protected:
  QString startSize = "0.16";
  QGraphicsSimpleTextItem *titleItem;

public slots:
  virtual void updateAll(){};

private:
  int type = 0;
  Qt::Key numKey = Qt::Key_0;

};
#endif/*__VISUTEST_ABSTRACTCHART_H__*/
