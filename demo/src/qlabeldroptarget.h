/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QLabel>

class QLabelDropTarget : public QLabel
{
  Q_OBJECT
public:
  explicit QLabelDropTarget(QWidget *parent = nullptr);
protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
};
