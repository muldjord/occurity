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

#include "videoplayer.h"

#include <stdio.h>

#include <QVideoFrame>
#include <QFileInfo>
#include <QDir>
#include <QKeyEvent>
#include <QBuffer>
#include <QUrl>

VideoPlayer::VideoPlayer(const QString &videosPath, const int &width, const int &height, QWidget *parent)
  : QWidget(parent)
{
  setCursor(Qt::BlankCursor);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  setStyleSheet("QVideoWidget {background-color: black;}");
  setFixedSize(width, height);

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

  mediaPlayer = new QMediaPlayer();
  //mediaPlayer->setAudioOutput(new QAudioOutput);
  QVideoSink *sink = new QVideoSink(this);
  setVideoSink(sink);
  mediaPlayer->setVideoSink(sink);
  if(!videoBuffers.isEmpty()) {
    videoIdx = 0;
    mediaPlayer->setSourceDevice(videoBuffers.at(videoIdx), QUrl("video/mp4"));
  }
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

void VideoPlayer::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  if(!m_image.isNull()) {
    p.drawImage(rect(), m_image);
  }
}

void VideoPlayer::setVideoSink(QVideoSink *sink)
{
  connect(sink, &QVideoSink::videoFrameChanged, this, &VideoPlayer::onNewFrame);
}

void VideoPlayer::onNewFrame(const QVideoFrame &frame) {
  QVideoFrame f(frame);
  if(!f.isValid())
    return;

  f.map(QVideoFrame::ReadOnly);
  QImage img = f.toImage();
  f.unmap();

  if(!img.isNull()) {
    m_image = img.copy();
    update();
  }
}

void VideoPlayer::keyPressEvent(QKeyEvent *event)
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

void VideoPlayer::playPressed()
{
  printf("Starting video playback!\n");
  mediaPlayer->play();
  /*
  allowStop = false;
  QTimer::singleShot(2000, [=]() { allowStop = true; });
  }
  */
}

void VideoPlayer::pausePressed()
{
  if(mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
    printf("Pausing video playback!\n");
    mediaPlayer->pause();
  } else {
    printf("Starting video playback after pause!\n");
    mediaPlayer->play();
  }
}

void VideoPlayer::stopPressed()
{
  printf("Stopping video playback!\n");
  mediaPlayer->stop();
  hide();
}

void VideoPlayer::nextPressed()
{
  videoIdx++;

  if(videoIdx >= videoBuffers.length())
    videoIdx = 0;

  printf("Changing to video buffer %d\n", videoIdx);

  mediaPlayer->setSourceDevice(videoBuffers.at(videoIdx), QUrl("video/mp4"));
  mediaPlayer->play();
}
