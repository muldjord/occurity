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

OptoSymbol::OptoSymbol(const QString &optoSymbol,
                       const QString &fadeTimings,
                       const QString &fadeLevels)
  : QGraphicsSvgItem(optoSymbol)
{
  opacityEffect = new QGraphicsOpacityEffect;
  opacityEffect->setOpacity(0.0);
  setGraphicsEffect(opacityEffect);

  fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeInAnimation->setDuration(fadeTimings.count(';') != 3?0:fadeTimings.split(";").at(0).toInt());
  //fadeInAnimation->setStartValue(0.0);
  fadeInAnimation->setEndValue(fadeLevels.count(';') != 2?1.0:fadeLevels.split(";").at(0).toDouble());
  fadeInAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  fadeOutAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeOutAnimation->setDuration(fadeTimings.count(';') != 3?50:fadeTimings.split(";").at(1).toInt());
  //fadeOutAnimation->setStartValue(1.0);
  fadeOutAnimation->setEndValue(fadeLevels.count(';') != 2?0.0:fadeLevels.split(";").at(1).toDouble());
  fadeOutAnimation->setEasingCurve(QEasingCurve::InOutQuad);
  connect(fadeOutAnimation, &QPropertyAnimation::finished, this, &OptoSymbol::fadeDoneHide);

  QPropertyAnimation *fadeInOutInAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeInOutInAnimation->setDuration(fadeTimings.count(';') != 3?150:fadeTimings.split(";").at(2).toInt());
  fadeInOutInAnimation->setStartValue(0.0);
  fadeInOutInAnimation->setEndValue(fadeLevels.count(';') != 2?0.15:fadeLevels.split(";").at(2).toDouble());
  fadeInOutInAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  QPropertyAnimation *fadeInOutOutAnimation = new QPropertyAnimation(opacityEffect, "opacity");
  fadeInOutOutAnimation->setDuration(fadeTimings.count(';') != 3?150:fadeTimings.split(";").at(3).toInt());
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
