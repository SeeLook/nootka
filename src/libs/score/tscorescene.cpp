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

#include "tscorescene.h"
#include "tscoremeter.h"
#include "tscoremeasure.h"
#include <tnoofont.h>
#include <music/trhythm.h>
#include <music/tmeter.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>


TscoreScene::TscoreScene(QObject* parent) :
  QGraphicsScene(parent),
  m_nameColor(Qt::darkCyan),
  m_accidYoffset(0.0),
  m_accidScale(-1.0),
  m_scoreNote(0),
  m_scoreMeter(nullptr)
{
  setDoubleAccidsEnabled(true);
  m_currentAccid = 0;

}


TscoreScene::~TscoreScene()
{
  delete m_scoreMeter;
}


void TscoreScene::setCurrentAccid(char accid) {
  char prevAcc = m_currentAccid;
  m_currentAccid = (char)qBound((int)-m_dblAccFuse, (int)accid, (int)m_dblAccFuse);
}


void TscoreScene::setDoubleAccidsEnabled(bool enable) {
  if (enable)
    m_dblAccFuse = 2;
  else
    m_dblAccFuse = 1;
}


void TscoreScene::addBlur(QGraphicsItem* item, qreal radius) {
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
//   blur->setBlurRadius(radius / views()[0]->transform().m11());
  item->setGraphicsEffect(blur);
}


void TscoreScene::setScoreMeter(TscoreMeter* m) {
  m_scoreMeter = m;
}














