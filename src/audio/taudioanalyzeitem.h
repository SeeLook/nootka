/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#ifndef TAUDIOANALYZEITEM_H
#define TAUDIOANALYZEITEM_H


#include <QtQuick/qquickitem.h>


class TcommonListener;
class TnoteStruct;


/**
 * This is a proxy QML item with C++ logic of audio/sound note data,
 */
class TaudioAnalyzeItem : public QQuickItem
{

  Q_OBJECT

public:
  explicit TaudioAnalyzeItem(QQuickItem* parent = nullptr);
  ~TaudioAnalyzeItem();

signals:
  void dataReady(qreal pcmVol, qreal energy, bool onSet);
  void noteData(int start, int end, const QString& note);

protected:
  void soundInitSlot();
  void volumeSlot();
  void noteFinishedSlot(const TnoteStruct& n);

private:
  TcommonListener           *m_sniffer = nullptr;
};

#endif // TAUDIOANALYZEITEM_H
