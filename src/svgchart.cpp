/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            svgchart.cpp
 *
 *  Tue Sep 25 14:00:00 UTC+1 2018
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

#include "svgchart.h"

#include <stdio.h>
#include <math.h>

#include <QKeyEvent>
#include <QFileInfo>
#include <QSvgRenderer>

SvgChart::SvgChart(MainSettings &mainSettings, QObject *parent) :
  AbstractChart(mainSettings, parent)
{
}

SvgChart::~SvgChart()
{
}

void SvgChart::init()
{
  svgItem = new QGraphicsSvgItem();
  addItem(svgItem);

  titleItem->setText(objectName());
  addItem(titleItem);

  updateAll();
}

void SvgChart::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Left) {
    if(!layers.isEmpty()) {
      svgIdx++;
      if(svgIdx >= layers.length()) {
        svgIdx = 0;
      }
      svgItem->setElementId(layers.at(svgIdx));
    }
  } else if(event->key() == Qt::Key_Right) {
    if(!layers.isEmpty()) {
      svgIdx--;
      if(svgIdx < 0) {
        svgIdx = layers.length() - 1;
      }
      svgItem->setElementId(layers.at(svgIdx));
    }
  }
  updateAll();
}

void SvgChart::setSource(const QString &source)
{
  QFile svgFile(source);
  if(svgFile.open(QIODevice::ReadOnly)) {
    svgXml = svgFile.readAll();
    svgXmlOrig = svgXml;
    svgFile.close();
  } else {
    printf("Couldn't open svg file '%s' for reading.\n", source.toStdString().c_str());
  }
}

void SvgChart::setRedGreen(const QString hexRed, const QString hexGreen)
{
  svgXml = svgXmlOrig;
  svgXml.replace("fill:#d20000", "fill:" + hexRed.toUtf8());
  svgXml.replace("fill:#00d200", "fill:" + hexGreen.toUtf8());
}

void SvgChart::setScaling(const QString &scaling)
{
  this->scaling = scaling;
}

bool SvgChart::addSvgLayer(const QString &svgLayerId)
{
  layers.append(svgLayerId);
  return true;
}

void SvgChart::updateAll()
{
  setRedGreen(mainSettings.hexRed, mainSettings.hexGreen);

  svgItem->renderer()->load(svgXml);

  if(!layers.isEmpty() && svgIdx <= layers.size())
    svgItem->setElementId(layers.at(svgIdx));
  else
    svgItem->setElementId("");

  if(scaling == "width") {
    svgItem->setScale(mainSettings.width / svgItem->renderer()->defaultSize().width());
  } else if(scaling == "height") {
    svgItem->setScale((double)mainSettings.height / svgItem->renderer()->defaultSize().height());
  } else if(scaling == "distance") {
    svgItem->setScale((mainSettings.pxPerArcMin / 100.0) * mainSettings.distanceFactor);
  }
  svgItem->setX((mainSettings.width / 2.0) - (svgItem->mapRectToScene(svgItem->boundingRect()).width()) / 2.0);
  svgItem->setY((mainSettings.height / 2.0) - (svgItem->mapRectToScene(svgItem->boundingRect()).height()) / 2.0);

  // Re-add title
  QFont font;
  font.setFamily("Arial");
  font.setPixelSize(mainSettings.pxPerArcMin * mainSettings.distanceFactor * 1.5);
  titleItem->setFont(font);
  titleItem->setX(mainSettings.width - titleItem->boundingRect().width() - 10);
  titleItem->setY(mainSettings.height - titleItem->boundingRect().height() - 10);

  // Update entire screen area
  update(0, 0, mainSettings.width, mainSettings.height);
}
