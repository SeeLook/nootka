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
#include "tscorecontrol.h"
#include <QDebug>
#include <QGraphicsProxyWidget>

TsimpleScore::TsimpleScore(QWidget* parent) :
  QGraphicsView(parent)
{
  setGeometry(parent->geometry());
  
  setMouseTracking(true);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  
  m_scene = new TscoreScene(this);
  connect(m_scene, SIGNAL(statusTip(QString)), this, SLOT(statusTipChanged(QString)));
  setScene(m_scene);
  
  
  m_staff = new TscoreStaff(m_scene, 3, TscoreStaff::e_normal);
  
  m_scoreControl = new TscoreControl(m_scene);
  m_scoreControl->proxy()->setPos(m_staff->boundingRect().width(), 0);
  
  
}

TsimpleScore::~TsimpleScore()
{}


int TsimpleScore::heightForWidth(int w ) const {
  return w * 8;
}

void TsimpleScore::resizeEvent(QResizeEvent* event) {
  qreal factor = ((qreal)height() / 40.0) / transform().m11();
//   factor = factor / 3;
  scale(factor, factor);
  m_scoreControl->setFixedSize(50, m_staff->boundingRect().height() * factor);
}

