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

TouchControlItem::TouchControlItem(const QPixmap &pixmap, const int &key, AbstractChart *chart)
  : QGraphicsPixmapItem(pixmap), key(key), chart(chart)
{
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlag(QGraphicsItem::ItemIsSelectable);
}

TouchControlItem::~TouchControlItem()
{
}

void TouchControlItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && key != -1) {
    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier);
    qApp->postEvent(chart, keyEvent);
  }
}
