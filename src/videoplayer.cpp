/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            videoplayer.cpp
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

#include "videoplayer.h"

#include <stdio.h>

#include <QKeyEvent>
#include <QFileInfo>

VideoPlayer::VideoPlayer(const int &width, const int &height, QWidget *parent) :
  QVideoWidget(parent)
{
  setFixedSize(width, height);
  setWindowFlags(Qt::WindowStaysOnTopHint);
  mediaPlayer = new QMediaPlayer();
  mediaPlayer->setVideoOutput(this);
  if(QFileInfo::exists("./video.mp4")) {
    QFile videoFile("./video.mp4");
    if(videoFile.open(QIODevice::ReadOnly)) {
      videoData = videoFile.readAll();
      printf("Loaded %d bytes of video data from 'video.mp4'\n", videoData.length());
      videoFile.close();
    }
  }
  videoBuffer = new QBuffer();
  videoBuffer->setData(videoData);
  videoBuffer->open(QIODevice::ReadOnly);
  mediaPlayer->setMedia(QMediaContent(), videoBuffer);
  allowStopTimer.setInterval(4000);
  allowStopTimer.setSingleShot(true);
  connect(&allowStopTimer, &QTimer::timeout, this, &VideoPlayer::setAllowStop);
}

VideoPlayer::~VideoPlayer()
{
  videoBuffer->close();
}

void VideoPlayer::startVideo()
{
  printf("Starting video!\n");
  show();
  mediaPlayer->play();
  allowStopTimer.start();
}

void VideoPlayer::stopVideo()
{
  if(allowStop) {
    printf("Stopping video!\n");
    allowStop = false;
    mediaPlayer->stop();
    hide();
  }
}

void VideoPlayer::setAllowStop()
{
  allowStop = true;
}
