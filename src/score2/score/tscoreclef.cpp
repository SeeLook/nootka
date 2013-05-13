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

#include "tscoreclef.h"
#include "tscorescene.h"
#include "tclef.h"
// #include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>

/*static*/
QChar TscoreClef::clefToChar(Tclef clef) {
  QChar ch;
  switch(clef.type()) {
    case Tclef::e_none:
      ch = QChar(0); break;
    case Tclef::e_treble_G:
      ch = QChar(0xe172); break;
    case Tclef::e_bass_F:
      ch = QChar(0xe170); break;
    case Tclef::e_alto_C:
      ch = QChar(0xe16e); break;
    case Tclef::e_treble_G_8down:
      ch = QChar(0xe173); break;
    case Tclef::e_bass_F_8down:
      ch = QChar(0xe171); break;
    case Tclef::e_tenor_C:
      ch = QChar(0xe16e); break;  
  }
  return ch;
}

QList<Tclef::Etype> TscoreClef::m_typesList = QList<Tclef::Etype>();



TscoreClef::TscoreClef(TscoreScene* scene, TscoreStaff* staff, Tclef clef) :
  TscoreItem(scene),
  m_clef(Tclef(Tclef::e_none)),
  m_textClef(0)
{
  setStaff(staff);
  if (m_typesList.size() == 0) // initialize types list
    m_typesList << Tclef::e_treble_G << Tclef::e_bass_F << Tclef::e_bass_F_8down << Tclef::e_alto_C << Tclef::e_tenor_C
    << Tclef::e_treble_G_8down;
  
  m_textClef = new QGraphicsSimpleTextItem();
  registryItem(m_textClef);
  m_textClef->setBrush(scene->views()[0]->palette().windowText().color());
  
  QFont f("nootka");
  f.setPixelSize(8);
  m_textClef->setFont(f);
  
  setClef(clef);
  
}
  

TscoreClef::~TscoreClef()
{
}


void TscoreClef::setClef(Tclef clef) {
  if (clef.type() != m_clef.type()) { 
    m_clef = clef;
    m_currClefInList = getClefPosInList(m_clef);
    m_textClef->setText(QString(clefToChar(m_clef.type())));
    setPos(1, getYclefPos(m_clef));
    setStatusTip(m_clef.name());
    emit statusTip(statusTip());
  }
}


QRectF TscoreClef::boundingRect() const {
  if (m_textClef)
      return m_textClef->boundingRect();
  else
      return QRectF(0, 0, 6, 40);
}

void TscoreClef::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}



void TscoreClef::wheelEvent(QGraphicsSceneWheelEvent* event) {
  if (event->delta() > 0) {
    m_currClefInList++;
    if (m_currClefInList >= m_typesList.size())
      m_currClefInList = 0;
  } else {
    m_currClefInList--;
    if (m_currClefInList < 0)
      m_currClefInList = m_typesList.size() - 1;
  }
  setClef(Tclef(m_typesList[m_currClefInList]));
  emit clefChanged(m_clef.type());
}


//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

int TscoreClef::getYclefPos(Tclef clef) {
  int pos = 0;
  if (clef.type() == Tclef::e_treble_G || clef.type() == Tclef::e_treble_G_8down)
    pos = 12;
  else if (clef.type() == Tclef::e_bass_F|| clef.type() == Tclef::e_bass_F_8down)
    pos = 8;
  else if (clef.type() == Tclef::e_alto_C)
    pos = 10;
  else if (clef.type() == Tclef::e_tenor_C)
    pos = 8;
  return pos;
}


int TscoreClef::getClefPosInList(Tclef clef) {
  for (int i = 0; i < m_typesList.size(); i++)
    if (m_typesList[i] == clef.type()) {
      return i;
    }
}




