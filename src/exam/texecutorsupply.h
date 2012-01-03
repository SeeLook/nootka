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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TEXECUTORSUPPLY_H
#define TEXECUTORSUPPLY_H

#include "tnote.h"
#include <QObject>

class TexamLevel;

/** A purpose of this class is to make the exam executor smaller.
 * Also it has eventFilter() reimplementation to manage right mouse button,
 * witch can be simply captured by contextMenuEvent from disabled widgets. */
class TexecutorSupply : public QObject
{
  Q_OBJECT
public:
  TexecutorSupply(TexamLevel *level, QObject *parent = 0);
  
  void createQuestionsList();
  Tnote determineAccid(Tnote n);
        /** */
  Tnote forceEnharmAccid(Tnote n);
  
signals:
  void rightButtonClicked();
  
protected:
  virtual bool eventFilter(QObject* obj, QEvent* event);

  
private:
  TexamLevel *m_level;
  
      /** Previous accidentals used*/
  Tnote::Eacidentals m_prevAccid;
      /** intervals between asking about double accidentals.
        * By default every forth question is with double adccid.*/
  int m_dblAccidsCntr;
        /** to switch nameing style between "do re mi" and "c d e"
        * when question and answer are note name.*/
  bool m_isSolfege;
};

#endif // TEXECUTORSUPPLY_H
