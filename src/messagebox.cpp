/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            messagebox.cpp
 *
 *  Thu Mar 24 12:21:00 UTC+1 2022
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

#include "messagebox.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QAbstractButton>

MessageBox::MessageBox(const QMessageBox::Icon &icon, const QString &title, const QString &text, const QMessageBox::StandardButtons &buttons, QWidget *parent)
  : QMessageBox(icon, title, text, buttons, parent)
{
  installEventFilter(this);
  for(auto *button: this->buttons()) {
    if(buttonRole(button) == QMessageBox::NoRole ||
       buttonRole(button) == QMessageBox::RejectRole) {
      button->setFocus();
      break;
    }
  }
}

bool MessageBox::eventFilter(QObject *, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent->key() == Qt::Key_Enter ||
       keyEvent->key() == Qt::Key_Return ||
       keyEvent->key() == Qt::Key_R) {
      for(auto *button: buttons()) {
        if(button->hasFocus()) {
          if(buttonRole(button) == QMessageBox::YesRole ||
             buttonRole(button) == QMessageBox::AcceptRole) {
            accept();
            return true;
          } else if(buttonRole(button) == QMessageBox::NoRole ||
                    buttonRole(button) == QMessageBox::RejectRole) {
            reject();
            return false;
          }
          break;
        }
      }
    } else if(keyEvent->key() == Qt::Key_Escape) { // 'R' is the default enter key on the remote
      reject();
      return true;
    }
  }
  return false;
}
