/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tpixmaker.h"
#include <QFont>
#include <QFontMetricsF>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QBuffer>
#include <QDebug>


QPixmap pixFromString(QString glif, QFont font) {
  QFontMetricsF metrics(font);
  QPixmap pix(metrics.width(glif), metrics.height());
  pix.fill(Qt::transparent);
  QPainter painter(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setWindow(0, 0, pix.width(), pix.height());
  painter.setPen(qApp->palette().text().color());
  painter.setBrush(QBrush(Qt::NoBrush));
  painter.setFont(font);
//  painter.drawRect(0, 0, pix.width(), pix.height());
  painter.drawText(0, 0, pix.width(), pix.height(), Qt::AlignCenter, glif);
  return pix;
}

QString pixToHtml(QString imageFile, int width) {
  if (!width)
    return QString("<img src=\"%1\">").arg(imageFile);
  else {
    QPixmap orgPix;
    if (!orgPix.load(imageFile))
      return "";
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    QPixmap scaledPix = orgPix.scaled(width, width);
    scaledPix.save(&buffer, "PNG");
    return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
  }
}

