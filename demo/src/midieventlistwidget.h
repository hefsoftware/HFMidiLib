/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QListWidget>
#include <HFMidiLib/port.h>

class MidiEventListWidget : public QListWidget
{
  Q_OBJECT
public:
  MidiEventListWidget(QWidget *parent=nullptr);

  // QWidget interface
  void setPort(const QSharedPointer<HFMidi::Port> &port);
protected slots:
  void onMidiEvent(QByteArray data);
protected:
  QMetaObject::Connection m_connection;
  QSharedPointer<HFMidi::Port> m_port;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
};
