/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainsettings.h
 *
 *  Mon Apr 9 14:00:00 UTC+1 2018
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

#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include "videoplayer.h"

#include <QString>

struct MainSettings {
  int hibernateTime = 7200000; // 2 hours
  int width = 1920;
  int height = 1080;
  int rowSkipDelta = 4;
  QString pinCode = "4242";
  QString jobsFolder = "./jobs";
  bool updatePretend = false;
  double patientDistance = 600.0;
  double distanceFactor = 1.0;
  double rulerWidth = 138.0;
  double pxPerMm = 42.0;
  //QString startSize = "1.0";
  double pxPerArcMin = 0.0;
  QString optotypesDir = "optotypes";
  QString chartsXml = "charts.xml";
  QString sizeFormat = "decimal";
  QString hexRed = "#d20000";
  QString hexGreen = "#00d200";
  bool enableVideoPlayer = true;
  QString videosFolder = "./videos";
  
  // Global chart configs
  bool crowding = false;
  bool single = false;
};

#endif // MAINSETTINGS_H
