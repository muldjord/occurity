/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            about.cpp
 *
 *  Tue Mar 13 17:00:00 UTC+1 2018
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

#include "about.h"

#include <stdio.h>
#include <QTextEdit>
#include <QFile>

About::About(QWidget *parent) : QTabWidget(parent)
{
  setMinimumWidth(600);
  setStyleSheet("QTextEdit {background-color: white; padding: 15px;}");

  QTextEdit *aboutText = new QTextEdit("<img src=\":icon.png\"/>" + tr("<h1>Occurity v") + "" VERSION"" + tr("</h1><h3>by Lars Bisballe</h3><h4>https://www.github.com/muldjord/Occurity</h4><p>Occurity is a visual acuity test software designed for use on the RaspBerry Pi hardware platform.It enables you to built an affordable visual acuity test system using a few pieces of hardware and basically any remote control. If you do not have the option to buy the infrared reciever, it can also be used with a keyboard.</p><h4>General keyboard functions</h4><p><ul><li>q: Enable / disable standby</li><li>j: Launch job runner (requires pin-code)</li><li>p: Launch preferences (requires pin-code)</li><li>s: Restart current attention video</li><li>d: Play / pause current attention video</li><li>f: Stop attention video</li><li>g: Play next attention video</li></ul></p><h4>Optotype chart specific keyboard functions</h4><ul><li>Up: Change to next line with bigger size</li><li>Down: Change to next line with smaller size</li><li>Left: Move entire row to the left</li><li>Right: Move entire row to the right</li><li>PgUp: Skip n number of lines bigger (configure n in preferences)</li><li>PgDn: Skip n number of lines smaller (configure n in preferences)</li><li>c: Enable / disable crowding</li><li>m: Switch between single and multisymbol from the selected row. Use left / right to move between them</li><li>r: Randomize symbols from selected row</li><li>a: Enable / disable attention animation (if one is defined)</li></ul></p><h4>SVG chart specific keyboard functions</h4><p><ul><li>left / right: Switch between layers defined in charts.xml</li><li></li><li></li><li></li><li></li><li></li><li></li><ul></p><p>Using a remote control reciever that emulates a keyboard, it is possible to control Occurity with basically any remote control. The Flirc USB has been tested to work well: https://flirc.tv/more/flirc-usb"));
  aboutText->setWordWrapMode(QTextOption::WordWrap);
  aboutText->setReadOnly(true);

  QFile file("LICENSE");
  QByteArray gplText = "";
  if(file.open(QIODevice::ReadOnly)) {
    gplText = file.readAll();
    file.close();
  } else {
    printf("ERROR: Couldn't find LICENSE file at the designated location.\n");
    gplText = "ERROR: File not found... This means that someone has been fiddling with the files of this software and someone might be violating the terms of the GPL. Go to the following location to read the license: http://www.gnu.org/licenses/gpl-3.0.html";
  }

  QTextEdit *licenseText = new QTextEdit(gplText);
  licenseText->setWordWrapMode(QTextOption::WordWrap);
  //licenseText->setStyleSheet("QTextEdit {background-color: white; padding: 15px;}");

  addTab(aboutText, tr("About"));
  addTab(licenseText, tr("License"));
}

About::~About()
{
}
