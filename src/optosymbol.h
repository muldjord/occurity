/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            optosymbol.h
 *
 *  Tue Mar 13 17:00:00 UTC+1 2018
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
#ifndef __OCCURITY_OPTOSYMBOL_H__
#define __OCCURITY_OPTOSYMBOL_H__

#include <QGraphicsSvgItem>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>

class OptoSymbol : public QGraphicsSvgItem
{
public:
  OptoSymbol(const QString &optoSymbol,
             const QString &fadeTimings,
             const QString &fadeLevels);
  ~OptoSymbol();
  void fadeIn();
  void fadeOut();
  void fadeInOut();
  bool isHidden();

private slots:
  void fadeDoneHide();
  
private:
  QGraphicsOpacityEffect *opacityEffect;

  QPropertyAnimation *fadeInAnimation;
  QPropertyAnimation *fadeOutAnimation;

  QSequentialAnimationGroup *fadeInOutAnimation;

};
#endif/*__OCCURITY_OPTOSYMBOL_H__*/
