/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "miditestsource.h"
#include "ui_miditestsource.h"
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
MidiTestSource::MidiTestSource(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MidiTestSource)
{
  ui->setupUi(this);

    setAcceptDrops(true);
}


void MidiTestSource::dragEnterEvent(QDragEnterEvent *event)
{
  if(event->mimeData()->formats().contains("mididev/output"))
  {
    event->accept();
  }
}

void MidiTestSource::dragMoveEvent(QDragMoveEvent *event)
{

}
void MidiTestSource::setPort(const QSharedPointer<HFMidi::Port> &port)
{
  m_port=port;
}
void MidiTestSource::dropEvent(QDropEvent *event)
{
  auto port=HFMidi::openPort(event->mimeData()->data("mididev/output"));
  if(port!=m_port)
    setPort(port);
}


MidiTestSource::~MidiTestSource()
{
  delete ui;
}

void MidiTestSource::on_aNote_toggled(bool checked)
{
  if(m_port)
  {
    if(checked)
      m_port->sendEvent(QByteArray("\x90\x47\x54", 3));
    else
      m_port->sendEvent(QByteArray("\x80\x47\x70", 3));
  }
}
void MidiTestSource::on_timeSignature_clicked()
{
  if(m_port)
    m_port->sendEvent(QByteArray("\xF0\x43\x10\x4C\x08\x00\x0C\x00\xF7", 9)); // Unknown event from Yamaha silent piano change instrument
//  90 39 49
//  80 39 40
//  F0 43 10 4C 08 00 07 00 F7
//  B0 00 6C
//  B0 20 00
//  C0 10
//  F0 43 10 4C 02 01 00 04 18 F7
//  F0 43 10 4C 02 01 20 00 00 F7
//  B0 07 3E
//  B0 5B 07
//  F0 43 10 4C 08 00 0C 00 F7
//  F0 43 10 4C 08 00 0D 72 F7
//  90 39 40
//  80 39 40
//  F0 43 10 4C 08 00 07 00 F7
//  B0 00 6C
//  B0 20 00
//  C0 00
//  F0 43 10 4C 02 01 00 01 18 F7
//  F0 43 10 4C 02 01 20 00 00 F7
//  B0 07 64
//  B0 5B 09
//  F0 43 10 4C 08 00 0C 40 F7
//  F0 43 10 4C 08 00 0D 40 F7
//  90 39 39
//  80 39 40
}
