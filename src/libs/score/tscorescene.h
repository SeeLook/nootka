/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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

#ifndef TSCORESCENE_H
#define TSCORESCENE_H

#include <nootkacoreglobal.h>
#include "tscorenote.h"
#include "tnoteitem.h"
#include "tscorelines.h"
#include <QtWidgets/qgraphicsscene.h>


class QGraphicsItem;
class TscoreMeter;
class Tmeter;


/**
 * This is subclass of QGraphicsScene that handles additional operations of Nootka score.
 */
class NOOTKACORE_EXPORT TscoreScene : public QGraphicsScene
{

  friend class TscoreNote;
  friend class TscoreStaff;

  Q_OBJECT

public:
  TscoreScene(QObject* parent = 0);
  virtual ~TscoreScene();


  void setDoubleAccidsEnabled(bool enable);
      /** Returns value 2 when double accidentals are enabled and 1 if not. */
  qint8 doubleAccidsFuse() { return m_dblAccFuse; }

  void setCurrentAccid(char accid); /**< Working accidental in  also changed by buttons. */
  char currentAccid() { return m_currentAccid; }

      /** Adds blur graphics effect. In the contrary to QGraphicsItem::setGraphicsEffect()
        * a radius value in global scale.  */
  void addBlur(QGraphicsItem *item, qreal radius);

  void setNameColor(const QColor& nameC) { m_nameColor = nameC; }
  QColor nameColor() { return m_nameColor; }

      /** Y offset of accidental item */
  static qreal accidYoffset() { return m_accidYoffset; }

      /** Scale of accidental text item */
  static qreal accidScale() { return m_accidScale; }

  bool isRhythmEnabled() { return (bool)m_scoreMeter; } /**< @p TRUE when score has rhythm enabled. */
  TscoreMeter* scoreMeter() { return m_scoreMeter; } /**< Score meter - if it is @p nullptr - there is no rhythms */

  void noteDeleted(TscoreNote* sn); /**< From @p TscoreNote::~TscoreNote() */
  TscoreNote* currentNote() { return m_scoreNote; }

signals:
  void statusTip(QString);

protected:
      /** Sets meter, enables rhythms if meter is valid or disables rhythm if it is @p nullptr.
       * This is global meter for all score items and it is managed through @class TscoreStaff */
  void setScoreMeter(TscoreMeter* m);


protected slots:
  void statusTipChanged(QString status) { emit statusTip(status); }

private:
      /** It is @p 2 if double accidentals are enabled and @p 1 if not*/
  qint8                             m_dblAccFuse;
  char                              m_currentAccid;
  QColor                            m_nameColor;
  static const qreal                m_accidYoffset; /**< difference between y note position. */
  static const qreal                m_accidScale;
  TscoreNote                       *m_scoreNote; /**< current note segment or NULL. */
  TscoreMeter                      *m_scoreMeter;
};

#endif // TSCORESCENE_H
