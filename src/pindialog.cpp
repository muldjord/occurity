/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            pindialog.cpp
 *
 *  Fri Feb 25 13:53:00 UTC+1 2022
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

#include "pindialog.h"

#include <QKeyEvent>
#include <QHBoxLayout>
#include <QTimer>

PinDialog::PinDialog(const QString &correctPinCode, QWidget *parent)
  : QDialog(parent), correctPinCode(correctPinCode)
{
  setWindowTitle(tr("Pincode:"));
  QHBoxLayout *layout = new QHBoxLayout;
  for(int a = 0; a < correctPinCode.length(); ++a) {
    QLabel *asteriskLabel = new QLabel;
    asteriskLabel->setPixmap(QPixmap(":pinblank.png"));
    asterisks.append(asteriskLabel);
    layout->addWidget(asterisks.last());
  }
  setLayout(layout);
}

void PinDialog::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_0 ||
     event->key() == Qt::Key_1 ||
     event->key() == Qt::Key_2 ||
     event->key() == Qt::Key_3 ||
     event->key() == Qt::Key_4 ||
     event->key() == Qt::Key_5 ||
     event->key() == Qt::Key_6 ||
     event->key() == Qt::Key_7 ||
     event->key() == Qt::Key_8 ||
     event->key() == Qt::Key_9) {
    if(event->key() == Qt::Key_0) {
      pinCode.append("0");
    } else if(event->key() == Qt::Key_1) {
      pinCode.append("1");
    } else if(event->key() == Qt::Key_2) {
      pinCode.append("2");
    } else if(event->key() == Qt::Key_3) {
      pinCode.append("3");
    } else if(event->key() == Qt::Key_4) {
      pinCode.append("4");
    } else if(event->key() == Qt::Key_5) {
      pinCode.append("5");
    } else if(event->key() == Qt::Key_6) {
      pinCode.append("6");
    } else if(event->key() == Qt::Key_7) {
      pinCode.append("7");
    } else if(event->key() == Qt::Key_8) {
      pinCode.append("8");
    } else if(event->key() == Qt::Key_9) {
      pinCode.append("9");
    }
    asterisks.at(pinCode.length() - 1)->setPixmap(QPixmap(":pinfilled.png"));
  }
  if(pinCode.length() == correctPinCode.length()) {
    if(pinCode == correctPinCode) {
      for(auto *asterisk: asterisks) {
        asterisk->setPixmap(QPixmap(":pincorrect.png"));
      }
      QTimer::singleShot(500, this, &PinDialog::accept);
    } else {
      for(auto *asterisk: asterisks) {
        asterisk->setPixmap(QPixmap(":pinincorrect.png"));
      }
      shakeHead();
    }
  }
}

void PinDialog::shakeHead()
{
  if(headShakes < 8) {
    move(pos().x() + (headShakes % 2 == 0?10:-10), pos().y());
    headShakes++;
    QTimer::singleShot(75, this, &PinDialog::shakeHead);
    return;
  }
  reject();
}
