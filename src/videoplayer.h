/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            videoplayer.h
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
#ifndef __OCCURITY_VIDEOPLAYER_H__
#define __OCCURITY_VIDEOPLAYER_H__

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QTimer>
#include <QLabel>
#include <QBuffer>
#include <QPainter>
#include <QImage>

class VideoPlayer : public QWidget
{
  Q_OBJECT

public:
  VideoPlayer(const QString &videosPath,
              const int &width, const int &height,
              QWidget *parent = nullptr);
  ~VideoPlayer();
  void setVideoSink(QVideoSink *sink);
  void playPressed();

protected:
  void paintEvent(QPaintEvent *) override;
  void keyPressEvent(QKeyEvent *event) override;

private slots:
  void onNewFrame(const QVideoFrame &frame);

private:
  QImage m_image;
  void pausePressed();
  void stopPressed();
  void nextPressed();

  int videoIdx = -1;

  QMediaPlayer *mediaPlayer = nullptr;
  QList<QBuffer *> videoBuffers;

};
#endif/*__OCCURITY_VIDEOPLAYER_H__*/
