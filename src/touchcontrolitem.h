/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            touchcontrolitem.h
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
#ifndef __OCCURITY_TOUCHCONTROLITEM_H__
#define __OCCURITY_TOUCHCONTROLITEM_H__

#include "abstractchart.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class TouchControlItem : public QGraphicsPixmapItem
{
public:
  TouchControlItem(const QPixmap &pixmap, const int &key, AbstractChart *parent);
  ~TouchControlItem();

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  
private:
  int key = -1;
  AbstractChart *chart = nullptr;
  
};
#endif/*__OCCURITY_TOUCHCONTROLITEM_H__*/
