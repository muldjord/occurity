/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            touchcontrols.cpp
 *
 *  Wed Jan 4 23:53:00 UTC+1 2023
 *  Copyright 2023 Lars Bisballe
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

#include "touchcontrols.h"

#include <QApplication>
#include <QKeyEvent>
#include <QGridLayout>
#include <QPushButton>

TouchControls::TouchControls(const bool &numeric, QObject *target)
  : target(target)
{
  setWindowTitle(tr("Touch controls"));
  setFocusPolicy(Qt::NoFocus);

  QGridLayout *layout = new QGridLayout;
  if(numeric) {
    QPushButton *zeroButton = new QPushButton(tr("0"), this);
    zeroButton->setObjectName("zero");
    connect(zeroButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *oneButton = new QPushButton(tr("1"), this);
    oneButton->setObjectName("one");
    connect(oneButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *twoButton = new QPushButton(tr("2"), this);
    twoButton->setObjectName("two");
    connect(twoButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *threeButton = new QPushButton(tr("3"), this);
    threeButton->setObjectName("three");
    connect(threeButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *fourButton = new QPushButton(tr("4"), this);
    fourButton->setObjectName("four");
    connect(fourButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *fiveButton = new QPushButton(tr("5"), this);
    fiveButton->setObjectName("five");
    connect(fiveButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *sixButton = new QPushButton(tr("6"), this);
    sixButton->setObjectName("six");
    connect(sixButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *sevenButton = new QPushButton(tr("7"), this);
    sevenButton->setObjectName("seven");
    connect(sevenButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *eightButton = new QPushButton(tr("8"), this);
    eightButton->setObjectName("eight");
    connect(eightButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *nineButton = new QPushButton(tr("9"), this);
    nineButton->setObjectName("nine");
    connect(nineButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    layout->addWidget(zeroButton, 3, 0);
    layout->addWidget(oneButton, 2, 0);
    layout->addWidget(twoButton, 2, 1);
    layout->addWidget(threeButton, 2, 2);
    layout->addWidget(fourButton, 1, 0);
    layout->addWidget(fiveButton, 1, 1);
    layout->addWidget(sixButton, 1, 2);
    layout->addWidget(sevenButton, 0, 0);
    layout->addWidget(eightButton, 0, 1);
    layout->addWidget(nineButton, 0, 2);
  } else {
    QPushButton *upButton = new QPushButton(tr("UP"), this);
    upButton->setObjectName("up");
    connect(upButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *downButton = new QPushButton(tr("DOWN"), this);
    downButton->setObjectName("down");
    connect(downButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *leftButton = new QPushButton(tr("LEFT"), this);
    leftButton->setObjectName("left");
    connect(leftButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    QPushButton *rightButton = new QPushButton(tr("RIGHT"), this);
    rightButton->setObjectName("right");
    connect(rightButton, &QPushButton::pressed, this, &TouchControls::sendKey);
    layout->addWidget(upButton, 0, 0);
    layout->addWidget(downButton, 1, 0);
    layout->addWidget(leftButton, 0, 1);
    layout->addWidget(rightButton, 1, 1);
  }

  for(auto *button: findChildren<QPushButton *>()) {
    button->setFocusPolicy(Qt::NoFocus);
  }
  setLayout(layout);
}

void TouchControls::sendKey()
{
  for(const auto *button: findChildren<QPushButton *>()) {
    if(!button->isDown()) {
      continue;
    }
    QKeyEvent *keyEvent = nullptr;
    if(button->objectName() == "up") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    } else if(button->objectName() == "down") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    } else if(button->objectName() == "left") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
    } else if(button->objectName() == "right") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
    } else if(button->objectName() == "enter") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    } else if(button->objectName() == "zero") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier);
    } else if(button->objectName() == "one") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    } else if(button->objectName() == "two") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    } else if(button->objectName() == "three") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier);
    } else if(button->objectName() == "four") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_4, Qt::NoModifier);
    } else if(button->objectName() == "five") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_5, Qt::NoModifier);
    } else if(button->objectName() == "six") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_6, Qt::NoModifier);
    } else if(button->objectName() == "seven") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_7, Qt::NoModifier);
    } else if(button->objectName() == "eight") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_8, Qt::NoModifier);
    } else if(button->objectName() == "nine") {
      keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_9, Qt::NoModifier);
    }
    if(keyEvent != nullptr) {
      QApplication::sendEvent(target, keyEvent);
    }
  }
}
