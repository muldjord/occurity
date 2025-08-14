/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            videochart.cpp
 *
 *  Thu Aug 07 09:00:00 UTC+1 2025
 *  Copyright 2025 Lars Bisballe
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

#include "videochart.h"

#include <stdio.h>

#include <QAudioOutput>
#include <QFileInfo>
#include <QDir>
#include <QKeyEvent>
#include <QUrl>

VideoChart::VideoChart(MainSettings &mainSettings, QObject *parent)
  : AbstractChart(mainSettings, parent)
{
}

VideoChart::~VideoChart()
{
  for(auto *videoBuffer: videoBuffers) {
    if(videoBuffer->isOpen()) {
      videoBuffer->close();
    }
    delete videoBuffer;
  }
}

void VideoChart::init()
{
  setBgColor("#000000");

  mediaPlayer = new QMediaPlayer();
  mediaPlayer->setAudioOutput(new QAudioOutput);

  videoItem = new QGraphicsVideoItem();
  videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
  addItem(videoItem);
  videoItem->setSize(QSizeF(mainSettings.width, mainSettings.height));

  mediaPlayer->setVideoOutput(videoItem);
  //connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &VideoChart::mediaStatusChanged);
  connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &VideoChart::playbackStateChanged);

  updateAll();

  nextPressed();
}

void VideoChart::keyPressEvent(QKeyEvent *event)
{
  if(!videoBuffers.isEmpty()) {
    if(event->key() == Qt::Key_S) {
      // Play current video
      playPressed();
    } else if(event->key() == Qt::Key_D) {
      // Pause / play current video
      pausePressed();
    } else if(event->key() == Qt::Key_F) {
      // Stop video and reset to beginning
      stopPressed();
    } else if(event->key() == Qt::Key_G) {
      // Switch to next video
      nextPressed();
    }
  }
}

void VideoChart::makeIdle()
{
  printf("Stopping video playback!\n");
  mediaPlayer->stop();
}

void VideoChart::setVideosPath(const QString &videosPath)
{
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
        printf("Loaded %llu bytes of video data from '%s'\n",
               videoBuffer->buffer().length(),
               videoInfo.absoluteFilePath().toStdString().c_str());
        videoFile.close();
        videoBuffers.append(videoBuffer);
      }
    }
  }
}

void VideoChart::playPressed()
{
  printf("Starting video playback!\n");
  mediaPlayer->play();
  /*
  allowStop = false;
  QTimer::singleShot(2000, [=]() { allowStop = true; });
  }
  */
}

void VideoChart::pausePressed()
{
  if(mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
    printf("Pausing video playback!\n");
    mediaPlayer->pause();
  } else {
    printf("Starting video playback after pause!\n");
    mediaPlayer->play();
  }
}

void VideoChart::stopPressed()
{
  printf("Stopping video playback!\n");
  mediaPlayer->stop();
}

void VideoChart::nextPressed()
{
  bool wasPlaying = false;
  if(mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
    wasPlaying = true;
  }

  videoIdx++;

  if(videoIdx >= videoBuffers.length())
    videoIdx = 0;

  if(videoIdx < 0)
    videoIdx = videoBuffers.length() - 1;

  printf("Changing to video buffer %d\n", videoIdx);

  mediaPlayer->setSourceDevice(videoBuffers.at(videoIdx), QUrl("video/mp4"));

  if(wasPlaying) {
    printf("Starting video playback!\n");
    mediaPlayer->play();
  }
}

/*
void VideoChart::mediaStatusChanged(QMediaPlayer::MediaStatus status)
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
*/

void VideoChart::playbackStateChanged(QMediaPlayer::PlaybackState state)
{
  printf("STATE: %d\n", state);
}

void VideoChart::updateAll()
{
  videoItem->setX(0.0);
  videoItem->setY(0.0);

  update(0, 0, mainSettings.width, mainSettings.height);
}
