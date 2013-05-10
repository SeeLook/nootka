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
#include <QDebug>
#include <QPainter>

/*static*/
QChar TscoreClef::clefToChar(Tclef::Etype clef) {
  QChar ch;
  switch(clef) {
    case Tclef::e_none:
      ch = QChar(0); break;
    case Tclef::e_treble_G:
      ch = QChar(0xe172); break;
    case Tclef::e_bass_F:
      ch = QChar(0xe170); break;
    case Tclef::e_alt_C:
      ch = QChar(0xe16e); break;
    case Tclef::e_treble_G_8down:
      ch = QChar(0xe1a7); break;  
  }
//   qDebug() /*<< QString(ch) << ch.digitValue()*/ << (int)clef ;
  return ch;
}

QList<Tclef::Etype> TscoreClef::m_typesList = QList<Tclef::Etype>;



TscoreClef::TscoreClef(TscoreScene* scene, Tclef clef) :
  TscoreItem(scene),
  m_clef(Tclef(Tclef::e_none)),
  m_textClef(0)
{
  if (m_typesList.size() == 0) // initialize types list
    m_typesList << Tclef::e_treble_G << Tclef::e_bass_F << Tclef::e_alt_C << Tclef::e_treble_G_8down << Tclef::e_bass_F_8down;
  
  m_textClef = new QGraphicsSimpleTextItem();
  scene->addItem(m_textClef);
  m_textClef->setParentItem(this);
  
  QFont f("nootka");
  f.setPixelSize(8);
  m_textClef->setFont(f);
  
  setClef(clef);
  setPos(1, 10); // alto
//   setPos(1, 12); // treble
//   setPos(1, 8); // bass
  
  setStatusTip(tr("This is a clef"));
}
  

TscoreClef::~TscoreClef()
{
}


void TscoreClef::setClef(Tclef clef) {
  if (clef.type() != m_clef.type()) { 
    m_clef = clef;
    m_textClef->setText(QString(clefToChar(m_clef.type())));
  }
}


QRectF TscoreClef::boundingRect() const {
  if (m_textClef)
      return m_textClef->boundingRect();
  else
      return QRectF(0, 0, 6, 40);
}

void TscoreClef::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
//   painter->setBrush(QBrush(QColor(5, 5, 5, 100)));
//   painter->drawRect(boundingRect());
  
}

void TscoreClef::wheelEvent(QGraphicsSceneWheelEvent* event) {
//   emit clefChanged();
}



