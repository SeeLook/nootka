/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#ifndef TCOMBINEDANIM_H
#define TCOMBINEDANIM_H

#include <nootkacoreglobal.h>
#include "tabstractanim.h"
#include "tmovedanim.h"
#include "tscaledanim.h"
#include "tcoloredanim.h"
#include "tmorphedanim.h"
#include "tfadeanim.h"


/**
 * Class that performs combined animations.
 *
 * WARRING! duration has to be set before any animation type is initialized.
 */
class NOOTKACORE_EXPORT TcombinedAnim : public TabstractAnim
{
    Q_OBJECT

public:
  explicit TcombinedAnim(QGraphicsItem* item = 0, QObject* parent = 0);

  void startAnimations();

  void setMoving(const QPointF& start, const QPointF& stop);
  TmovedAnim* moving() { return m_moving; }

  void setScaling(qreal scaleEnd, qreal scaleMid = -1.0);
  TscaledAnim* scaling() { return m_scaling; }

  void setColoring(const QColor &endColor, const QColor &midColor = -1);
  TcoloredAnim* coloring() { return m_coloring; }

  void setMorphing(const QLineF &line, qreal width, bool toLine = true);
  TmorphedAnim* morphing() { return m_morphing; }

  void setFading(qreal endOpacity, qreal midOpacity = -1);
  TfadeAnim* fading() { return m_fading; }

protected slots:
  void finishSlot();
  void emitFinish() { emit finished(); }

private:
      /** Common routines for all kinds of animations */
  void prepareAnim(TabstractAnim *anim);

private:
  TmovedAnim        *m_moving;
  QPointF             m_startMov, m_stopMov;
  TscaledAnim        *m_scaling;
  qreal               m_scaleEnd, m_scaleMid;
  TcoloredAnim      *m_coloring;
  QColor             m_endColor, m_midColor;
  TmorphedAnim      *m_morphing;
  QLineF             m_line;
  qreal               m_lineWidth;
  TfadeAnim          *m_fading;
  qreal               m_endOp, m_midOp;
  bool               m_toLine;

};

#endif // TCOMBINEDANIM_H
