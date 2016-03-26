/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

#ifndef TCROSSFADETEXTITEM_H
#define TCROSSFADETEXTITEM_H

#include <nootkacoreglobal.h>
#include <QObject>
#include <QColor>

#define CLIP_TIME (30) // every move per 30 ms

class QGraphicsSimpleTextItem;

/**
* Crossfading animation of passing two texts of given QGraphicsSimpleTextItem.
*/
class NOOTKACORE_EXPORT TcrossFadeTextAnim : public QObject
{
    Q_OBJECT

public:

  TcrossFadeTextAnim(QGraphicsSimpleTextItem* textItem, QObject *parent = 0);

  void setDuration(int duration) { m_duration = duration; m_maxStep = m_duration / CLIP_TIME; }

signals:
    void crossFadingFinished();

public slots:
  void startCrossFading(const QString &newText, const QColor &newColor = -1);


protected slots:
  void crossFadingSlot();

private:
      /** Sets alpha value of item brush color.  */
  void setItemAlpha(int alpha);

private:
  QGraphicsSimpleTextItem    *m_item;
  int                         m_maxStep, m_currStep, m_duration, m_alpha;
  QString                     m_newText;
  QColor                     m_newColor;

};

#endif // TCROSSFADETEXTITEM_H
