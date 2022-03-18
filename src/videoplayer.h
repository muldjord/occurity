/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            videoplayer.h
 *
 *  Fri Mar 18 17:00:00 UTC+1 2022
 *  Copyright 2022 Lars Bisballe
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
#ifndef __VISUTEST_VIDEOPLAYER_H__
#define __VISUTEST_VIDEOPLAYER_H__

#include <QVideoWidget>
#include <QMediaPlayer>
#include <QBuffer>

class VideoPlayer : public QVideoWidget
{
  Q_OBJECT

public:
  VideoPlayer(const int &width, const int &height, QWidget *parent = nullptr);
  ~VideoPlayer();
  void startVideo();
  void stopVideo();

private:
  QMediaPlayer *mediaPlayer = nullptr;
  QByteArray videoData = "";
  QBuffer *videoBuffer = nullptr;
  /*
protected:
  void keyPressEvent(QKeyEvent *event) override;
  */

};
#endif/*__VISUTEST_VIDEOPLAYER_H__*/
