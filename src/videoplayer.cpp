/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            videoplayer.cpp
 *
 *  Fri Mar 18 17:00:00 UTC+1 2022
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

#include "videoplayer.h"

#include <stdio.h>

#include <QFileInfo>
#include <QDir>
#include <QKeyEvent>

VideoPlayer::VideoPlayer(const QString &videosPath,
                         const int &width, const int &height,
                         QWidget *parent) :
  QVideoWidget(parent)
{
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  setStyleSheet("QVideoWidget {background-color: black;}");
  setFixedSize(width, height);

  mediaPlayer = new QMediaPlayer();
  QDir videosDir(videosPath, "*.mp4", QDir::Name, QDir::Files);
  for(const auto &videoInfo: videosDir.entryInfoList()) {
    if(QFileInfo::exists(videoInfo.absoluteFilePath())) {
      QFile videoFile(videoInfo.absoluteFilePath());
      if(videoFile.open(QIODevice::ReadOnly)) {
        QPair<QString, QByteArray> videoData;
        videoData.first = videoInfo.fileName();
        videoData.second = videoFile.readAll();
        qInfo("Loaded %d bytes of video data from '%s'\n",
              videoData.second.length(),
              videoInfo.absoluteFilePath().toStdString().c_str());
        videoFile.close();
        videosData.append(videoData);
      }
    }
  }
  mediaPlayer->setVideoOutput(this);
  videoBuffer = new QBuffer();

  allowActionTimer.setInterval(4000);
  allowActionTimer.setSingleShot(true);
  connect(&allowActionTimer, &QTimer::timeout, this, &VideoPlayer::setAllowStop);
}

VideoPlayer::~VideoPlayer()
{
  videoBuffer->close();
}

void VideoPlayer::changeVideo(const int &delta)
{
  videoIdx = videoIdx + delta;
  if(videoIdx >= videosData.length()) {
    videoIdx = 0;
  }
  if(videoIdx < 0) {
    videoIdx = videosData.length() - 1;
  }
  if(videoBuffer->isOpen()) {
    videoBuffer->close();
  }
  videoBuffer->setData(videosData.at(videoIdx).second);
  videoBuffer->open(QIODevice::ReadOnly);
  mediaPlayer->setMedia(QMediaContent(), videoBuffer);
  startVideo();
}

void VideoPlayer::startVideo()
{
  printf("Started!\n");
  if(videoIdx == -1) {
    changeVideo(1);
    return;
  }
  show();
  if(mediaPlayer->state() == QMediaPlayer::PlayingState) {
    mediaPlayer->pause();
  } else {
    mediaPlayer->play();
    allowAction = false;
    allowActionTimer.start();
  }
}

void VideoPlayer::stopVideo()
{
  if(allowAction) {
    mediaPlayer->stop();
    hide();
  }
}

void VideoPlayer::setAllowStop()
{
  allowAction = true;
}

void VideoPlayer::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_S) {
    changeVideo(-1);
  } else if(event->key() == Qt::Key_G) {
    changeVideo(1);
  } else if(event->key() == Qt::Key_D) {
    startVideo();
  } else if(event->key() == Qt::Key_F) {
    stopVideo();
  }
}
