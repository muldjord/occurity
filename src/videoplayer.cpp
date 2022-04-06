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
  int videosFound = videosDir.entryInfoList().length();
  for(const auto &videoInfo: videosDir.entryInfoList()) {
    if(QFileInfo::exists(videoInfo.absoluteFilePath())) {
      if(videosFound > 2 && videoInfo.fileName() == "example.mp4") {
        continue;
      }
      QFile videoFile(videoInfo.absoluteFilePath());
      if(videoFile.open(QIODevice::ReadOnly)) {
        QBuffer *videoBuffer = new QBuffer();
        videoBuffer->buffer() = videoFile.readAll();
        printf("Loaded %d bytes of video data from '%s'\n",
              videoBuffer->buffer().length(),
              videoInfo.absoluteFilePath().toStdString().c_str());
        videoFile.close();
        videoBuffers.append(videoBuffer);
      }
    }
  }
  mediaPlayer->setVideoOutput(this);
  connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &VideoPlayer::mediaStatusChanged);
  connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &VideoPlayer::stateChanged);

  allowActionTimer.setInterval(4000);
  allowActionTimer.setSingleShot(true);
  connect(&allowActionTimer, &QTimer::timeout, this, &VideoPlayer::setAllowStop);
}

VideoPlayer::~VideoPlayer()
{
  for(auto *videoBuffer: videoBuffers) {
    if(videoBuffer->isOpen()) {
      videoBuffer->close();
    }
    delete videoBuffer;
  }
}

void VideoPlayer::changeVideo(const int &delta)
{
  if(videoIdx != -1 && videoBuffers.at(videoIdx)->isOpen()) {
    videoBuffers.at(videoIdx)->close();
  }
  videoIdx += delta;
  if(videoIdx >= videoBuffers.length()) {
    videoIdx = 0;
  }
  if(videoIdx < 0) {
    videoIdx = videoBuffers.length() - 1;
  }
  printf("Change to %d\n", videoIdx);
  if(videoBuffers.at(videoIdx)->open(QIODevice::ReadOnly)) {
    mediaPlayer->setMedia(QMediaContent(), videoBuffers.at(videoIdx));
  }
}

void VideoPlayer::startVideo()
{
  if(!videoBuffers.isEmpty()) {
    if(videoIdx == -1) {
      changeVideo(1);
    }
  } else {
    return;
  }
  if(mediaPlayer->state() == QMediaPlayer::PlayingState) {
    printf("Pausing!\n");
    mediaPlayer->pause();
  } else {
    printf("Starting!\n");
    show();
    mediaPlayer->play();
    allowAction = false;
    allowActionTimer.start();
  }
}

void VideoPlayer::stopVideo()
{
  if(allowAction) {
    printf("Stopping!\n");
    mediaPlayer->pause();
    hide();
  }
}

void VideoPlayer::setAllowStop()
{
  allowAction = true;
}

void VideoPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
  printf("STATUS: %d\n", status);
  if(status == QMediaPlayer::EndOfMedia) {
    printf("END OF MEDIA! CHANGING VIDEO!\n");
    changeVideo(1);
  } else if(status == QMediaPlayer::LoadedMedia) {
    printf("MEDIA LOADED!\n");
    startVideo();
  }
}

void VideoPlayer::stateChanged(QMediaPlayer::State state)
{
  printf("STATE: %d\n", state);
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
