/***************************************************************************
 *            slider.h
 *
 *  Mon Feb 24 12:00:00 CEST 2014
 *  Copyright 2014 Lars Bisballe Jensen
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

#ifndef SLIDER_H
#define SLIDER_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QSettings>
#include <QKeyEvent>

class Slider : public QWidget
{
  Q_OBJECT

public:
  Slider(QSettings &config, const QString &group, const QString &name, const QString &title,
	 const int &minValue, const int &maxValue, const int &defaultValue, const int &step = 10,
	 QWidget *parent = nullptr);
  ~Slider();

public slots:
  void resetToDefault();

private slots:
  void saveToConfig();

private:
  QSettings &config;
  QString key;
  int defaultValue;

  QSlider *valueSlider;
  QLineEdit *valueLineEdit;
};

#endif
