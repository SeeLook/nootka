/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tsimplescore.h"
#include "tscorescene.h"
#include "tscorestaff.h"
#include <QDebug>

TsimpleScore::TsimpleScore(QWidget* parent) :
  QGraphicsView(parent)
{
  setGeometry(parent->geometry());
  m_scene = new TscoreScene(this);
  connect(m_scene, SIGNAL(statusTip(QString)), this, SLOT(statusTipChanged(QString)));
//   m_scene = new QGraphicsScene(this);
  setScene(m_scene);
  
  
  m_staff = new TscoreStaff(m_scene);
  m_scene->addScoreItem(m_staff);
  
  qreal factor = (qreal)height() / 40.0;
//   qDebug() << factor << geometry();
  scale(factor, factor);
  
//   setStatusTip("Hallo");
}

TsimpleScore::~TsimpleScore()
{}


int TsimpleScore::heightForWidth(int w ) const {
  return w * 8;
}
