/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            touchcontrols.h
 *
 *  Wed Jan 4 23:53:00 UTC+1 2023
 *  Copyright 2023 Lars Bisballe
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
#ifndef __OCCURITY_TOUCHCONTROLS_H__
#define __OCCURITY_TOUCHCONTROLS_H__

#include <QWidget>

class TouchControls : public QWidget
{
  Q_OBJECT

public:
  TouchControls(const bool &numeric, QObject *target);
  
private slots:
  void sendKey();

private:
  QObject *target = nullptr;

};
#endif/*__OCCURITY_TOUCHCONTROLS_H__*/
