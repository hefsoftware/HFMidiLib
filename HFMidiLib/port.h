/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QObject>
namespace HFMidi
{
  class Port: public QObject
  {
    Q_OBJECT
  public:
    Port();
  public slots:
    void sendEvent(const QByteArray &data);
  signals:
    void midiEvent(const QByteArray &data);
  protected:
    virtual bool sendData(const QByteArray &data);
    void receivedData(const QByteArray &data);
  };
  QSharedPointer<Port> openPort(const QString &id);
}
