/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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

#ifndef TBANDONEONBG_H
#define TBANDONEONBG_H


#include "tcommoninstrument.h"

#include <QtQuick/qquickitem.h>
#include <QtGui/qcolor.h>


class QQmlComponent;


/**
 * @class TbandoneonBg represents logic of bandoneon.
 * It has static array @p buttArray with X,Y button coordinates
 * and notes for bellows opening/closing for each of buttons.
 * From this array @p m_notesArray is created which contains number of @p buttArray elements for each note.
 * @p m_notesArray covers entire bandoneon scale (C-1 - note -11 to H3 - note 48) : 59 notes
 *
 * There are five circles that highlight selected buttons: some notes occur a few times (a few buttons)
 * depends on @p m_opening / @p m_closing state(s)
 */
class NOOTKACORE_EXPORT TbandoneonBg : public TcommonInstrument
{
  Q_OBJECT

  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)
  Q_PROPERTY(bool opening READ opening WRITE setOpening NOTIFY openingChanged)
  Q_PROPERTY(bool closing READ closing WRITE setClosing NOTIFY closingChanged)
  Q_PROPERTY(qreal rightX READ rightX WRITE setRightX NOTIFY rightXChanged)
  Q_PROPERTY(qreal factor READ factor WRITE setFactor NOTIFY factorChanged)
  Q_PROPERTY(EsideHighlight sideHighlight READ sideHighlight NOTIFY sideHighlightChanged)

public:
  TbandoneonBg(QQuickItem* parent = nullptr);
  ~TbandoneonBg();

  Q_INVOKABLE qreal xAt(int b);
  Q_INVOKABLE qreal yAt(int b);
  Q_INVOKABLE int openAt(int b);
  Q_INVOKABLE int closeAt(int b);

  int currentIndex() const { return m_currentIndex; }
  void setCurrentIndex(int i);

  bool opening() const { return m_opening; }
  void setOpening(bool o);

  bool closing() const { return m_closing; }
  void setClosing(bool c);

  void setNote(const Tnote& n, quint32 noteDataValue = 255) override;

  void askQuestion(const Tnote& n, quint32 noteDataValue) override;

  void highlightAnswer(const Tnote & n, quint32 noteData) override;

  int technical() override;

  qreal rightX() const { return m_rightX; }
  void setRightX(qreal rx);

  qreal factor() const { return m_factor; }
  void setFactor(qreal f);

  void paint(QPainter*) override {}

  void markSelected(const QColor & markColor) override;

  void showNoteName() override;

  void correct(const Tnote & n, quint32 noteData) override;

  Q_INVOKABLE void applyCorrect() override;

      /**
       * Returns @p TRUE when given note (as chromatic number @p Tnote::chromatic())
       * is possible to play on bandoneon at given side/bellow direction
       */
  bool canBeLeftOpen(short int chromNoteNr);
  bool canBeLeftClose(short int chromNoteNr);
  bool canBeRightOpen(short int chromNoteNr);
  bool canBeRightClose(short int chromNoteNr);

      /**
       * Describes which side of bandoneon has to be highlighted for an answer.
       * It depends on note layout (upper staff, lower staff)
       */
  enum EsideHighlight {
    HighlightNone = 0, HighlightLeft, HighlightRight
  };
  Q_ENUM(EsideHighlight)

  EsideHighlight sideHighlight() const { return m_sideHighlight; }

signals:
  void closingChanged();
  void openingChanged();
  void rightXChanged();
  void factorChanged();
  void sideHighlightChanged();

private:

      /**
       * @p QQuickItem circle and button number in @p buttArray, or 0 if no button.
       */
  class TbandCircle {
    public:
      int             buttonId = 0;
      QQuickItem*     item = nullptr;
  };

  void getNote();
  QQuickItem* createCircle(QQmlComponent* comp);
  void updateCircleSize(QQuickItem* it);
  void updateCircesPos();
  void checkCircle(int butNr, TbandCircle& c, bool visible = true);
  void hideCircles();

private:
      /**
       * Contains button number in @p buttArray for left/right keys and opening/closing states.
       * NOTICE: number is increased by 1, so @p 0 means: no button
       */
  class TbandoNote {
    public:
      quint8 leftOpen = 0;
      quint8 leftClose = 0;
      quint8 rightOpen = 0;
      quint8 rightClose = 0;
  };

  int               m_currentIndex;
  bool              m_closing = false;
  bool              m_opening = false;
  TbandoNote        m_notesArray[60];
  TbandCircle       m_circleLeftOpen, m_circleLeftClose, m_circleRightOpen, m_circleRightClose, m_circleCloseExtra;
  qreal             m_factor = 1.0;
  qreal             m_rightX = 0.0;
  EsideHighlight    m_sideHighlight = HighlightNone;
  quint32           m_goodTechn;
  Tnote             m_goodNote;
  int               m_goodButton;
};

#endif // TBANDONEONBG_H
