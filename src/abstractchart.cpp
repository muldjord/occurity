/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            abstractchart.cpp
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

#include "abstractchart.h"

#include <stdio.h>
#include <math.h>
#include <QKeyEvent>

AbstractChart::AbstractChart(MainSettings &mainSettings, QObject *parent) :
  QGraphicsScene(0, 0, mainSettings.width, mainSettings.height, parent), mainSettings(mainSettings)
{
  setBackgroundBrush(QBrush(Qt::white)); // Forces white chart background for dark-mode desktops
  titleItem = new QGraphicsSimpleTextItem();
  QFont font;
  font.setFamily("Arial");
  font.setPixelSize((mainSettings.pxPerArcMin * 5.0) * mainSettings.distanceFactor * 2.0);
  titleItem->setFont(font);
}

AbstractChart::~AbstractChart()
{
}

void AbstractChart::init()
{
}

void AbstractChart::setType(const QString &type)
{
  if(type == "optotype") {
    this->type = OPTOTYPECHART;
  } else if(type == "svg") {
    this->type = SVGCHART;
  } else if(type == "video") {
    this->type = VIDEOCHART;
  }
}

void AbstractChart::setTouchControls(const QString &controls)
{
  if(mainSettings.touchControls) {
    for(const auto &control: controls.split(",")) {
      touchControls.append(new TouchControlItem(control, this));
      addItem(touchControls.last());
    }
  }
}

QString AbstractChart::getType()
{
  if(type == OPTOTYPECHART) {
    return QString("optotype");
  } else if(type == SVGCHART) {
    return QString("svg");
  } else if(type == VIDEOCHART) {
    return QString("video");
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

void AbstractChart::setSize(const QString &sizeStr)
{
  this->sizeStr = sizeStr;
}

QString AbstractChart::getSize()
{
  return sizeStr;
}

void AbstractChart::setSizeLocked(const bool &sizeLocked)
{
  this->sizeLocked = sizeLocked;
}

bool AbstractChart::isSizeLocked()
{
  return sizeLocked;
}

void AbstractChart::updateTouchControls()
{
  if(mainSettings.touchControls) {
    double touchScaleFactor = 1.0;
    if(mainSettings.width < 1920) {
      touchScaleFactor = mainSettings.width / 1920.0;
      for(auto *control: touchControls) {
        control->setScale(touchScaleFactor);
      }
    }
    double touchX = 0;
    if(mainSettings.leftHandedOperator) {
      touchX = mainSettings.width * touchScaleFactor;
      for(const auto *control: touchControls) {
        touchX -= control->boundingRect().width() * touchScaleFactor;
      }
    }
    for(auto *control: touchControls) {
      control->setPos(touchX, 0);
      control->show();
      touchX += control->boundingRect().width() * touchScaleFactor;
    }
  } else {
    for(auto *control: touchControls) {
      control->hide();
    }
  }
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
