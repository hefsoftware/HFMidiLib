/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QWidget>
#include <HFMidiLib/port.h>
namespace Ui {
  class MidiTestSource;
}

class MidiTestSource : public QWidget
{
  Q_OBJECT

public:
  explicit MidiTestSource(QWidget *parent = nullptr);
  ~MidiTestSource();
  // QWidget interface
  void setPort(const QSharedPointer<HFMidi::Port> &port);
protected:
  QSharedPointer<HFMidi::Port> m_port;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
private slots:
  void on_aNote_toggled(bool checked);

  void on_timeSignature_clicked();

private:
  Ui::MidiTestSource *ui;
};
