/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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

#ifndef TNOTEITEM_H
#define TNOTEITEM_H


#include "nootkacoreglobal.h"
#include "music/trhythm.h"
#include <QtQuick/qquickitem.h>
#include <QtCore/qelapsedtimer.h>


class TstaffItem;
class TmeasureObject;
class TnotePair;
class Tnote;


#define STEM_HEIGHT (6.0) // default height of a note stem


/**
 * @class TnoteItem is @class QQuickItem derivative representing single note on the score.
 * It dynamically creates QML items: note head, alter (accidental) text, stem rectangle and rhythm flag.
 * A tie item form Tie.qml when necessary
 */
class NOOTKACORE_EXPORT TnoteItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(qreal notePosY READ notePosY NOTIFY notePosYchanged)
  Q_PROPERTY(qreal alterWidth READ alterWidth NOTIFY alterWidthChanged)
  Q_PROPERTY(int index READ index)
  Q_PROPERTY(TstaffItem* staffItem READ staff)
  Q_PROPERTY(qreal rightX READ rightX NOTIFY rightXChanged)
  Q_PROPERTY(bool hasTie READ hasTie NOTIFY hasTieChanged)

  friend class TscoreObject;
  friend class TstaffItem;
  friend class TmeasureObject;
  friend class TbeamObject;
  friend class TnotePair;

public:
  explicit TnoteItem(TstaffItem* staffObj = nullptr, TnotePair* wrapper = nullptr);
  ~TnoteItem() override;

  TstaffItem* staff() const { return m_staff; }
  void setStaff(TstaffItem* staffObj);

  TmeasureObject* measure() { return m_measure; }
  void setMeasure(TmeasureObject* m);

  Tnote* note() { return m_note; }
  void setNote(const Tnote& n);

  quint32 technical() const;
  void setTechnical(quint32 tech);

  qreal notePosY() const { return m_notePosY; }

  Q_INVOKABLE Trhythm rhythm() const;

      /**
       * Note number in the staff
       */
  int index() const;

  qreal stemHeight() const { return m_stemHeight; }
  void setStemHeight(qreal sh);

  qreal alterWidth() { return m_alter->width(); }


  QColor color() { return m_head->property("color").value<QColor>(); }
  void setColor(const QColor& c);

      /**
       * Overrides standard @p setX() method to shift note segment about accidental symbol width (if it is set).
       * It also updates tie (if any) and beam (for the last note in a beam group)
       */
  void setX(qreal xx);

      /**
       * shortcut to X coordinate of right note corner plus gap related to rhythm and staff gap factor
       */
  qreal rightX() const;

  bool hasTie() const;

  void setHeight(qreal hh);

      /**
       * Returns gap factor after this note item depends on current rhythm value
       */
  qreal rhythmFactor() const;

      /**
       * Returns position of the top of this note stem in staff coordinates
       */
  QPointF stemTop();

      /**
       * Prints to std out debug info about this note: [NOTE number] in color
       */
  char debug();

  qreal tieWidth();

  void setNoteNameVisible(bool nameVisible);

      /**
       * Static method that converts given rhythm into text of a note head
       */
  static QString getHeadText(const Trhythm& r);

  void setFingerNumber(int fiNr);
  Q_INVOKABLE void setStringNumber(int strNr);

  enum EbowDirection {
    BowUndefined = 0,
    BowDown = 2, /**< For bandoneon it is bellow opening */
    BowUp = 4 /**< For bandoneon it is bellow closing */
  };
  Q_ENUM(EbowDirection)

  Q_INVOKABLE TnoteItem::EbowDirection bowing() const;
  Q_INVOKABLE void setBowing(TnoteItem::EbowDirection bowDir);

      /**
       * If @p outLineColor differs from note color, outline is set over note head,
       * but when it is the same, outline is unset (what means reset the mark)
       */
  void markNoteHead(const QColor& outlineColor);

      /**
       * Returns Y coordinate of the note head for given note @p n.
       * But it doesn't take care of Scorek font overlay,
       * so to get proper text item position @p 15.0 has to be subtracted
       */
  qreal getHeadY(const Tnote& n);

signals:
  void noteChanged();
  void notePosYchanged();
  void alterWidthChanged();
  void rightXChanged();
  void hasTieChanged();

protected:
  QString getAccidText();
  QString getHeadText() const;
  QString getFlagText();

  void keySignatureChanged();

  QQuickItem* tie() { return m_tie; }
  QQuickItem* head() { return m_head; }
  QQuickItem* nameItem() { return m_name; }
  TnotePair* wrapper() { return m_wrapper; }

      /**
       * Moves note head X position by @p shift.
       * It is used in single note mode when note head has to centered in highlight
       */
  void shiftHead(qreal shift);

      /**
       * @p TnoteItem manages tie itself whenever tie state changes, by calling exactly this method.
       * Tie is simple Text QML item of tie symbol with horizontal scaling that determines tie width,
       * but tie is not aware about next note position as long as next note X coordinate
       * depends on this note width and rhythm factor.
       * So @p setX() method usually called when staff factor is changing, updates tie width.
       * @p tieWidth() returns desired tie width.
       * Also @p TnoteItem objects takes care about breaking tie among staves.
       * It sets tie glyph text to @p m_accidText when note with tie begins a staff
       */
  void checkTie();

  void updateDebug();

  void hoverEnterEvent(QHoverEvent* event) override;
  void hoverLeaveEvent(QHoverEvent*) override;
  void hoverMoveEvent(QHoverEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

private:

  TstaffItem                  *m_staff;
  TnotePair                   *m_wrapper;
  TmeasureObject              *m_measure;
  Tnote                       *m_note;
  qreal                        m_notePosY;
  QQuickItem                  *m_head, *m_alter, *m_stem, *m_flag;
  QVector<QQuickItem*>         m_upLines, m_loLines, m_underLoLines;
  qreal                        m_stemHeight;
  QQuickItem                  *m_tie = nullptr;
  QQuickItem                  *m_name = nullptr;
  QQuickItem                  *m_stringNumber = nullptr;
  QQuickItem                  *m_bowing = nullptr;
  QQuickItem                  *m_fingerNumber = nullptr;
  static QElapsedTimer         m_touchDuration;
  static TnoteItem            *m_heldNote;

  QQuickItem                  *m_debug;

private:
  QQuickItem* createAddLine();
  void updateAlter();
  void updateWidth();
  void updateNoteHead();
  void updateTieScale();
  void checkStem();
  void updateNamePos();
  void checkAddLinesVisibility();
};

#endif // TNOTEITEM_H
