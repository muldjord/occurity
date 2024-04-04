/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            svgchart.h
 *
 *  Tue Sep 25 14:00:00 UTC+1 2018
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
#ifndef __OCCURITY_SVGCHART_H__
#define __OCCURITY_SVGCHART_H__

#include "mainsettings.h"
#include "abstractchart.h"

#include <QGraphicsSvgItem>
#include <QDomDocument>

class SvgChart : public AbstractChart
{
  Q_OBJECT

public:
  SvgChart(MainSettings &mainSettings, QObject *parent);
  ~SvgChart();
  void init() override;
  void setRedGreen(const QString hexRed, const QString hexGreen);
  void setSource(const QString &source);
  void setScaling(const QString &scaling);
  bool addSvgLayer(const QString &svgLayerId);

public slots:
  void updateAll() override;

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  QByteArray svgXmlOrig = "";
  QByteArray svgXml = "";
  QString scaling = "width";
  QList<QString> layers;
  int svgIdx = 0;
  QGraphicsSvgItem *svgItem;

};
#endif/*__OCCURITY_SVGCHART_H__*/
