/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.h
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

#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__

#include <QWidget>
#include <QCheckBox>
#include <QSettings>
#include <QKeyEvent>

class CheckBox : public QCheckBox
{
  Q_OBJECT
    
public:
  CheckBox(QSettings &config, const QString &group, const QString &name, const QString &title, const bool &defaultValue, QWidget *parent = nullptr);
  ~CheckBox();
  void addConfigItem(QString text, QString value);

public slots:
  void resetToDefault();

private slots:
  void saveToConfig();

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  QSettings &config;
  QString key;
  bool defaultValue;
};

#endif // __CHECKBOX_H__
