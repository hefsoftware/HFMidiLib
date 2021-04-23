/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "midieventlistwidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
MidiEventListWidget::MidiEventListWidget(QWidget *parent): QListWidget(parent)
{
  setAcceptDrops(true);
}


void MidiEventListWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if(event->mimeData()->formats().contains("mididev/input"))
    event->accept();
  else
    event->ignore();
}

void MidiEventListWidget::dragMoveEvent(QDragMoveEvent *event)
{
  Q_UNUSED(event);
  event->accept();
}
void MidiEventListWidget::setPort(const QSharedPointer<HFMidi::Port> &port)
{
  if(m_connection)
  {
    qDebug()<<"Disconnect port connection";
    QObject::disconnect(m_connection);
  }
  m_port=port;
  if(port)
  {
    m_connection=connect(port.data(), &HFMidi::Port::midiEvent, this, &MidiEventListWidget::onMidiEvent);
    qDebug()<<"Connect"<<(bool)m_connection;
  }
}
void MidiEventListWidget::onMidiEvent(QByteArray data)
{
  if(data[0]!='\xFE')
  {
    qDebug()<<"Midi event"<<data;
    addItem(data.toHex());
  }
}
void MidiEventListWidget::dropEvent(QDropEvent *event)
{
  qDebug()<<"Opening port id="<<event->mimeData()->data("mididev/input");
  auto port=HFMidi::openPort(event->mimeData()->data("mididev/input"));
  if(port!=m_port)
  {
    setPort(port);
  }
}
