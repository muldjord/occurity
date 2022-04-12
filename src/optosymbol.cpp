/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            optosymbol.cpp
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

#include "optosymbol.h"

#include <stdio.h>

#include <QPropertyAnimation>

OptoSymbol::OptoSymbol(const QString &optoSymbol) : QGraphicsSvgItem(optoSymbol)
{
  opacityEffect = new QGraphicsOpacityEffect;
  opacityEffect->setOpacity(0.0);
  setGraphicsEffect(opacityEffect);

  fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeInAnimation->setDuration(50);
  //fadeInAnimation->setStartValue(0.0);
  fadeInAnimation->setEndValue(1.0);
  fadeInAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  fadeOutAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeOutAnimation->setDuration(50);
  //fadeOutAnimation->setStartValue(1.0);
  fadeOutAnimation->setEndValue(0.0);
  fadeOutAnimation->setEasingCurve(QEasingCurve::InOutQuad);
  connect(fadeOutAnimation, &QPropertyAnimation::finished, this, &OptoSymbol::fadeDoneHide);

  QPropertyAnimation *fadeInOutInAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeInOutInAnimation->setDuration(250);
  fadeInOutInAnimation->setStartValue(0.0);
  fadeInOutInAnimation->setEndValue(0.1);
  fadeInOutInAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  QPropertyAnimation *fadeInOutOutAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeInOutOutAnimation->setDuration(250);
  //fadeInOutOutAnimation->setStartValue(0.1);
  fadeInOutOutAnimation->setEndValue(0.0);
  fadeInOutOutAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  fadeInOutAnimation = new QSequentialAnimationGroup;
  fadeInOutAnimation->addAnimation(fadeInOutInAnimation);
  fadeInOutAnimation->addAnimation(fadeInOutOutAnimation);
  connect(fadeInOutAnimation, &QSequentialAnimationGroup::finished, this, &OptoSymbol::fadeDoneHide);
}

OptoSymbol::~OptoSymbol()
{
}

void OptoSymbol::fadeIn()
{
  show();
  fadeInAnimation->start();
}

void OptoSymbol::fadeOut()
{
  fadeOutAnimation->start();
}

void OptoSymbol::fadeInOut()
{
  fadeInOutAnimation->start();
}

void OptoSymbol::fadeDoneHide()
{
  hide();
}

bool OptoSymbol::isHidden()
{
  if(!isVisible() ||
     fadeOutAnimation->state() == QAbstractAnimation::Running ||
     fadeOutAnimation->state() == QAbstractAnimation::Paused ||
     fadeInOutAnimation->state() == QAbstractAnimation::Running ||
     fadeInOutAnimation->state() == QAbstractAnimation::Paused) {
    return true;
  }
  return false;
}
