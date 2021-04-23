/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "mainwindow.h"

#include <QApplication>
#include "HFMidiLib/framework.h"
#include <QDebug>
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  HFMidi::Framework::framework()->initialize();
//  QString id("linux//137:VMPK Output//in0:out");
//  qDebug()<<"Open port"<<id<<"result"<<Midi::framework()->openPort(id);
  MainWindow w;
  w.show();
  return a.exec();
}
