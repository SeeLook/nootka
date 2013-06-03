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
#include "tscorestaff.h"
#include "tclefselector.h"
#include "tclef.h"
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
  m_textClef(0),
  m_readOnly(false),
  m_selector(0),
  m_isClickable(true)
{
  setStaff(staff);
	setParentItem(staff);
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
  

TscoreClef::~TscoreClef() {}


void TscoreClef::setClef(Tclef clef) {
  if (clef.type() != m_clef.type()) { 
    m_clef = clef;
    m_currClefInList = getClefPosInList(m_clef);
    m_textClef->setText(QString(clefToChar(m_clef.type())));
    setPos(1, getYclefPos(m_clef) - (16 - staff()->upperLinePos()));
    setStatusTip(m_clef.name() + " (" + m_clef.desc() + ")<br>" + tr("To change clef click it or use mouse wheel."));
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
  if (m_readOnly)
    return;
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
  emit clefChanged();
}


void TscoreClef::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	if (m_isClickable && !m_selector) {
		if (readOnly()) // occurs in pano staf only
			m_selector = new TclefSelector(scoreScene(), Tclef(Tclef::e_pianoStaff));
		else // second parametr is current cleff to mark
			m_selector = new TclefSelector(scoreScene(), m_clef);
		m_selector->setParentItem(this);
		if (staff()->type() == TscoreStaff::e_lower)
				m_selector->setPos(-1.0, -17.0);
		else
				m_selector->setPos(-1.0, -2.0);
		connect(m_selector, SIGNAL(clefSelected(Tclef)), this, SLOT(clefSelected(Tclef)));
	} else
			TscoreItem::mousePressEvent(event);	
}


void TscoreClef::clefSelected(Tclef clef) {
	m_selector->deleteLater();
	m_selector = 0;
	if (clef.type() == Tclef::e_none)
		return;
	// This is hard logic that I love so much...
	// We supose readOnly() is pointing that staff is piano
	if (readOnly()) {
		if (clef.type() != Tclef::e_pianoStaff) { // user selected other clef than piano
			emit switchPianoStaff(clef); // this staff will be deleted and emited clef will set
    } else
			return; // when user selected piano staff again - do nothing
	} else // ordinary single staff
			if (clef.type() != Tclef::e_pianoStaff) { // simply set another clef
				if (clef.type() != m_clef.type()) {
						setClef(clef);
						emit clefChanged();
				}
			} else // demand to change it on piano staff
					emit switchPianoStaff(Tclef(Tclef::e_pianoStaff));
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




