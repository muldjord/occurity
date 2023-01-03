/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.cpp
 *
 *  Thu Dec 29 11:140:00 CEST 2022
 *  Copyright 2022 Lars Bisballe Jensen
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

#include <stdio.h>
#include "checkbox.h"

#include <QLabel>
#include <QVBoxLayout>

CheckBox::CheckBox(QSettings &config, const QString &group, const QString &name, const QString &title, const bool &defaultValue, QWidget *parent)
  : QCheckBox(parent), config(config), defaultValue(defaultValue)
{
  setText(title);
  installEventFilter(parent);

  key = (group != "General"?group + "/":"") + name;

  if(!config.contains(key)) {
    config.setValue(key, defaultValue);
  }

  setChecked(config.value(key, false).toBool());
}

CheckBox::~CheckBox()
{
}

void CheckBox::resetToDefault()
{
  setChecked(defaultValue);
}

void CheckBox::saveToConfig()
{
  config.setValue(key, isChecked());

  printf("Key '%s' saved to config with value '%s'\n", qPrintable(key), isChecked()?"true":"false");
}

void CheckBox::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Left ||
     event->key() == Qt::Key_Right) {
    toggle();
    saveToConfig();
  }
}

