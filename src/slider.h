/***************************************************************************
 *            slider.h
 *
 *  Mon Feb 24 12:00:00 CEST 2014
 *  Copyright 2014 Lars Bisballe Jensen
 *  larsbjensen@gmail.com
 ****************************************************************************/
/*
 *  This file is part of Visutest.
 *
 *  Visutest is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Visutest is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Visutest; if not, write to the Free Software
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
  Slider(QSettings *config, QString group, QString name,
	 int minValue, int maxValue, int stdValue, int step = 10, QWidget *parent = NULL);
  ~Slider();

public slots:
  void resetToDefault();

private slots:
  void saveToConfig();

private:
  QSlider *valueSlider;
  QLineEdit *valueLineEdit;

  QSettings *config;
  QString name;
  QString group;
  int defaultValue;
};

#endif
