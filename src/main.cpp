/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            main.cc
 *
 *  Mon Oct 02 17:00:00 UTC+1 2017
 *  Copyright 2017 Lars Bisballe
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
#include "mainwindow.h"

#include <QStyleFactory>
#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QDateTime>
#include <QSplashScreen>
#include <QEventLoop>
#include <QTimer>
#include <QTextStream>

void customMessageHandler(QtMsgType type, const QMessageLogContext&, const QString &msg)
{
  // Add timestamp to debug message
  QString txt = QDateTime::currentDateTime().toString("ddMMMyyyy hh:mm:ss");
  // Decide which type of debug message it is, and add string to signify it
  // Then append the debug message itself to the same string.
  switch (type) {
  case QtInfoMsg:
    txt += QString(": Info: %1").arg(msg);
    break;
  case QtDebugMsg:
    txt += QString(": Debug: %1").arg(msg);
    break;
  case QtWarningMsg:
    txt += QString(": Warning: %1").arg(msg);
    break;
  case QtCriticalMsg:
    txt += QString(": Critical: %1").arg(msg);
    break;
  case QtFatalMsg:
    txt += QString(": Fatal: %1").arg(msg);
    break;
  }

  printf("%s", txt.toStdString().c_str());

  // Set debug file
  QFile outFile("debug.log");
  outFile.open(QIODevice::Append);
  QTextStream ts(&outFile);
  if(txt.right(1) == "\n") {
    ts << txt;
  } else {
    ts << txt << Qt::endl;
    printf("\n");
  }
  outFile.close();
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  app.setStyle(QStyleFactory::create("Fusion"));

  // Install the custom debug message handler used by qDebug()
  qInstallMessageHandler(customMessageHandler);

  QDir::setCurrent(QApplication::applicationDirPath());

  QSettings config("config.ini", QSettings::IniFormat);

  QPixmap pixmap(":splash.png");
  QSplashScreen *splash = new QSplashScreen(pixmap);
  splash->setCursor(Qt::BlankCursor);
  splash->show();
  splash->showMessage("Running Occurity v" VERSION,
                      Qt::AlignLeft,
                      Qt::white);
  app.processEvents();
  if(config.value("showSplash", true).toBool()) {
    QEventLoop q;
    QTimer::singleShot(5000, &q, &QEventLoop::quit);
    q.exec();
  }

  QTranslator translator;
  translator.load("Occurity_" + QLocale::system().name());
  app.installTranslator(&translator);

  MainWindow mainWindow(config);
  mainWindow.show();
  splash->finish(&mainWindow);
  return app.exec();
}
