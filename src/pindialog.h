/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            pindialog.h
 *
 *  Fri Feb 25 13:53:00 UTC+1 2022
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
#ifndef __OCCURITY_PINDIALOG_H__
#define __OCCURITY_PINDIALOG_H__

#include <QDialog>
#include <QLabel>

class PinDialog : public QDialog
{
  Q_OBJECT

public:
  PinDialog(const QString &correctPinCode, const bool &showTouchControls, QWidget *parent = nullptr);
  bool pinCorrect();

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  QList<QLabel *> asterisks;
  const QString &correctPinCode;
  QString pinCode = "";

};
#endif/*__OCCURITY_PINDIALOG_H__*/
