/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            combobox.cpp
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
 *  the Free Software Foundation; either version 3 of the License, or
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
#include "combobox.h"

#include <QLabel>
#include <QVBoxLayout>

ComboBox::ComboBox(QSettings &config, const QString &group, const QString &name, const QString &title, QString defaultValue, QWidget *parent)
  : QWidget(parent), config(config), defaultValue(defaultValue)
{
  QLabel *titleLabel = new QLabel(title);
  QVBoxLayout *layout = new QVBoxLayout;
  comboBox = new QComboBox(this);
  comboBox->installEventFilter(parent);
  layout->addWidget(titleLabel); 
  layout->addWidget(comboBox);
  setLayout(layout);

  key = (group != "General"?group + "/":"") + name;

  if(!config.contains(key)) {
    config.setValue(key, defaultValue);
  }

  connect(comboBox, QOverload<int>::of(&QComboBox::activated), this, &ComboBox::itemSelected);
}

ComboBox::~ComboBox()
{
}

void ComboBox::addConfigItem(QString text, QString value)
{
  comboBox->addItem(text, value);
  for(int i = 0; i < comboBox->count(); ++i) {
    if(comboBox->itemData(i).toString() == config.value(key).toString()) {
      comboBox->setCurrentIndex(i);
    }
  }
}

void ComboBox::resetToDefault()
{
  for(int i = 0; i < comboBox->count(); ++i) {
    if(comboBox->itemData(i).toString() == defaultValue) {
      comboBox->setCurrentIndex(i);
    }
  }
}

void ComboBox::itemSelected(int)
{
  saveToConfig();
}

void ComboBox::saveToConfig()
{
  config.setValue(key, comboBox->currentData().toString());

  printf("Key '%s' saved to config with value '%s'\n", qPrintable(key), qPrintable(comboBox->currentData().toString()));
}

void ComboBox::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Left) {
    if(comboBox->currentIndex() > 0) {
      comboBox->setCurrentIndex(comboBox->currentIndex() - 1);
      saveToConfig();
    }
  } else if(event->key() == Qt::Key_Right) {
    if(comboBox->currentIndex() < comboBox->count() - 1) {
      comboBox->setCurrentIndex(comboBox->currentIndex() + 1);
      saveToConfig();
    }
  }
}
