/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QTreeWidget>

class MidiDevicesTree : public QTreeWidget
{
  Q_OBJECT
public:
  explicit MidiDevicesTree(QWidget *parent = nullptr);
public slots:
  void updateDevices();

protected:
  QString extraToHtml(const QMap<QString, QVariant> &data, const QString &header, const QString &headerValue, const QString &curTooltip=QString());
  QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const override;
};
