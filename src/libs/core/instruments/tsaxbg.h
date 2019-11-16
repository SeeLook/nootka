/***************************************************************************
 *   Copyright (C) 2017-2019 by Tomasz Bojczuk                             *
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

#ifndef TSAXBG_H
#define TSAXBG_H


#include "tcommoninstrument.h"
#include "music/tnote.h"
#include <QtQuick/qquickitem.h>


/**
 * @class TsaxBg defines fingering chart shapes of notes from a# (11) till c3 (49).
 * Every shape is represented by unique number obtained as a bit-mask of closed (1) or opened (0) saxophone flips.
 * 
 * It converts given @p Tnote into shape number.
 * QML calls @p flapNumber and by it current shape is obtained,
 * then corresponding note is found end @p noteChanged() signal is emitted
 */
class NOOTKACORE_EXPORT TsaxBg : public TcommonInstrument
{

  Q_OBJECT

  Q_PROPERTY(int flapNumber READ flapNumber WRITE setFlapNumber NOTIFY flapNumberChanged)
  Q_PROPERTY(int fingeringId READ fingeringId NOTIFY fingeringIdChanged)

public:
  TsaxBg(QQuickItem* parent = nullptr);
  ~TsaxBg();

  void setNote(const Tnote& n, quint32 noteDataValue = NO_TECHNICALS) override;

  void askQuestion(const Tnote& n, quint32 noteDataValue) override;

  void highlightAnswer(const Tnote&, quint32) override {}

  int technical() override { return NO_TECHNICALS; } // Fake - saxophone has no extra note data

  int flapNumber() const { return m_flapNumber; }

      /**
       * Every saxophone flaps when clicked sets this value to its number
       */
  void setFlapNumber(int fNr);

  quint32 fingeringId() const { return m_fingeringId; }

  void paint(QPainter*) override {}

  void markSelected(const QColor & markColor) override;

  void showNoteName() override;

  void correct(const Tnote & n, quint32 noteData) override;

signals:
  void flapNumberChanged();
  void fingeringIdChanged();


private:
  int           m_flapNumber;
  quint32       m_fingeringId = 0;
  quint32      *m_notesArray;
};

#endif // TSAXBG_H
