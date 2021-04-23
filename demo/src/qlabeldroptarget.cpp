/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "qlabeldroptarget.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
QLabelDropTarget::QLabelDropTarget(QWidget *parent) : QLabel(parent)
{
  setAcceptDrops(true);
}


void QLabelDropTarget::dragEnterEvent(QDragEnterEvent *event)
{
  QString label;
  label="<html><table>";
  Q_FOREACH(auto type, event->mimeData()->formats())
  {
    QByteArray data=event->mimeData()->data(type);
    if(data.size()>400)
      data=data.left(400);
    label+=tr("<tr><td><b>%1:</b></td><td>%2</td></tr>").arg(type, QString::fromLatin1(data));
  }
  label+="</table></html>";
  setText(label);
}
