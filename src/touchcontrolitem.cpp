/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            touchcontrolitem.cpp
 *
 *  Fri Dec 30 11:10:00 UTC+1 2022
 *  Copyright 2022 Lars Bisballe
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

#include "touchcontrolitem.h"

#include <stdio.h>

#include <QApplication>
#include <QKeyEvent>

TouchControlItem::TouchControlItem(const QString &control, QObject *chart)
  : chart(chart)
{
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlag(QGraphicsItem::ItemIsSelectable);

  if(control == "left") {
    key = Qt::Key_Left;
    setPixmap(QPixmap(":touch-left.png"));
  } else if(control == "right") {
    key = Qt::Key_Right;
    setPixmap(QPixmap(":touch-right.png"));
  } else if(control == "up") {
    key = Qt::Key_Up;
    setPixmap(QPixmap(":touch-up.png"));
  } else if(control == "down") {
    key = Qt::Key_Down;
    setPixmap(QPixmap(":touch-down.png"));
  } else if(control == "crowding") {
    key = Qt::Key_C;
    setPixmap(QPixmap(":touch-crowding.png"));
  } else if(control == "single") {
    key = Qt::Key_M;
    setPixmap(QPixmap(":touch-single.png"));
  } else if(control == "preferences") {
    key = Qt::Key_P;
    setPixmap(QPixmap(":touch-preferences.png"));
  }

  setZValue(1.0);

  opacityEffect = new QGraphicsOpacityEffect;
  opacityEffect->setOpacity(1.0);
  setGraphicsEffect(opacityEffect);

  activatedAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  activatedAnimation->setDuration(200.0);
  activatedAnimation->setStartValue(0.4);
  activatedAnimation->setEndValue(1.0);
  activatedAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

TouchControlItem::~TouchControlItem()
{
}

void TouchControlItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && key != -1) {
    activatedAnimation->start();
    QApplication::postEvent(chart, new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier));
  }
  ungrabMouse(); // Otherwise the 'Preferences' item will keep the mouse input indefinitely
}
