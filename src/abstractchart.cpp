/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            abstractchart.cpp
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

#include "abstractchart.h"

#include <stdio.h>
#include <math.h>
#include <QKeyEvent>

AbstractChart::AbstractChart(MainSettings *mainSettings, QObject *parent) :
  QGraphicsScene(0, 0, mainSettings->width, mainSettings->height, parent)
{
  this->mainSettings = mainSettings;
  titleItem = new QGraphicsSimpleTextItem();
  QFont font;
  font.setFamily("Arial");
  font.setPixelSize((mainSettings->pxPerArcMin * 5.0) * mainSettings->distanceFactor * 2.0);
  titleItem->setFont(font);
}

AbstractChart::~AbstractChart()
{
}

void AbstractChart::init()
{
}

void AbstractChart::setType(QString type)
{
  if(type == "font") {
    this->type = FONTCHART;
  } else if(type == "svg") {
    this->type = SVGCHART;
  } else if(type == "optotype") {
    this->type = OPTOTYPECHART;
  }
}

QString AbstractChart::getType()
{
  if(type == FONTCHART) {
    return QString("font");
  } else if(type == SVGCHART) {
    return QString("svg");
  } else if(type == OPTOTYPECHART) {
    return QString("optotype");
  }

  return QString("Invalid type");
}

void AbstractChart::setNumKey(Qt::Key numKey)
{
  this->numKey = numKey;
}

Qt::Key AbstractChart::getNumKey()
{
  return numKey;
}

QString AbstractChart::getOptotype()
{
  return QString("None");
}

void AbstractChart::setBgColor(QString color)
{
  if(color.left(1) == "#" && color.length() == 7) {
    bool converted = false;
    color.replace("#", "");
    short red = color.left(2).toInt(&converted, 16);
    short green = color.mid(2, 2).toInt(&converted, 16);
    short blue = color.right(2).toInt(&converted, 16);
    if(converted) {
      setBackgroundBrush(QColor(red, green, blue));
    } else {
      printf("Failed to set chart background color from value '%s'.\n", color.toStdString().c_str());
    }
  } else if(color == "black") {
    setBackgroundBrush(Qt::black);
  } else if(color == "white") {
    setBackgroundBrush(Qt::white);
  }
}

void AbstractChart::addRow(QString, QString)
{
}

void AbstractChart::addRowString(QString, QString)
{
}
