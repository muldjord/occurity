/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            about.cpp
 *
 *  Tue Mar 13 17:00:00 UTC+1 2018
 *  Copyright 2018 Lars Bisballe
 *  larsbjensen@gmail.com
 ****************************************************************************/

/*
 *  This file is part of VisuTest.
 *
 *  VisuTest is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  VisuTest is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VisuTest; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include "about.h"

#include <stdio.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFile>

About::About(QWidget *parent) : QTabWidget(parent)
{
  setMinimumWidth(600);

  QLabel *aboutText = new QLabel(tr("<strong>VisuTest v" VERSION" by Lars Bisballe</strong><br/><br/>VisuTest is a visual acuity test software designed for use on the RaspBerry Pi hardware platform. It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared reciever, it can also be used with a keyboard.<br/><br/>Please note that VisuTest is provided without fonts. These need to be acquired elsewhere."));
  aboutText->setWordWrap(true);
  aboutText->setMaximumWidth(400);

  // License tab
  // Read COPYING data from file
  QFile file("COPYING");
  QByteArray gplText;
  if(file.open(QIODevice::ReadOnly)) {
    gplText = file.readAll();
    file.close();
  } else {
    printf("ERROR: Couldn't find COPYING file at the designated location.\n");
    gplText = "ERROR: File not found... This means that someone has been fiddling with the files of this software, and someone might be violating the terms of the GPL. Go to the following location to read the license: http://www.gnu.org/licenses/gpl-3.0.html";
  }

  QLabel *licenseText = new QLabel(gplText);
  licenseText->setWordWrap(true);
  licenseText->setMaximumWidth(400);

  QScrollArea *licenseScroll = new QScrollArea;
  licenseScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  licenseScroll->setWidget(licenseText);

  addTab(aboutText, tr("About"));
  addTab(licenseScroll, tr("License"));
}

About::~About()
{
}
