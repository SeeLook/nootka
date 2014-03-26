/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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
#include <music/tclef.h>
#include "widgets/tselectclef.h"
#include <tnoofont.h>
#include <QGraphicsSceneHoverEvent>
#include <QApplication>
#include <QPalette>
#include <QGraphicsView>
#include <QDebug>

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
		default : 
			ch = QChar(0x20); break; // space
  }
  return ch;
}

QList<Tclef::Etype> TscoreClef::m_typesList = QList<Tclef::Etype>();



TscoreClef::TscoreClef(TscoreScene* scene, TscoreStaff* staff, Tclef clef) :
  TscoreItem(scene),
  m_clef(Tclef(Tclef::e_none)),
  m_textClef(0),
  m_readOnly(false),
  m_clefMenu(0)
{
  setStaff(staff);
	setParentItem(staff);
  if (m_typesList.size() == 0) // initialize types list
    m_typesList << Tclef::e_treble_G << Tclef::e_bass_F << Tclef::e_bass_F_8down <<
    Tclef::e_alto_C << Tclef::e_tenor_C << Tclef::e_treble_G_8down;
  
  m_textClef = new QGraphicsSimpleTextItem();
  registryItem(m_textClef);
  m_textClef->setBrush(qApp->palette().text().color());
  
  m_textClef->setFont(TnooFont(18));
  
  setClef(clef);
}
  

TscoreClef::~TscoreClef() {}


void TscoreClef::setClef(Tclef clef) {
  if (clef.type() != m_clef.type()) { 
    m_clef = clef;
    m_currClefInList = getClefPosInList(m_clef);
    m_textClef->setText(QString(clefToChar(m_clef.type())));
		qreal fineOff = 0.1;
		if (clef.type() == Tclef::e_bass_F || clef.type() == Tclef::e_bass_F_8down)
			fineOff = 0.0;
    setPos(1, getYclefPos(m_clef) - (16 - staff()->upperLinePos()) + fineOff);
    getStatusTip();
//     emit statusTip(statusTip());
  }
}


QRectF TscoreClef::boundingRect() const {
  if (m_textClef)
      return QRectF(0, 0, CLEF_WIDTH, m_textClef->boundingRect().height());
  else
      return QRectF(0, 0, 6, 40);
}

void TscoreClef::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}


void TscoreClef::mousePressEvent(QGraphicsSceneMouseEvent* event) {
		if (m_readOnly) {
			TscoreItem::mousePressEvent(event);
		} else {
			if (!m_clefMenu) {
					QMenu *menu = new QMenu(scoreScene()->views()[0]->parentWidget()->parentWidget());
					m_clefMenu = new TclefMenu(menu);
					Tclef curClef = m_clef;
					if (staff()->lower())
						curClef = Tclef(Tclef::e_pianoStaff);
					m_clefMenu->selectClef(curClef);
					connect(m_clefMenu, SIGNAL(statusTipRequired(QString)), this, SLOT(clefMenuStatusTip(QString)));
					Tclef cl = m_clefMenu->exec(event->screenPos());
					delete m_clefMenu;
					m_clefMenu = 0;
					delete menu;
					if (cl.type() == Tclef::e_none)
						return;
				// This is hard logic that I love so much...
					if (staff()->kindOfStaff() != TscoreStaff::e_normal) {
						if (cl.type() != Tclef::e_pianoStaff) { // user selected other clef than piano
								emit switchPianoStaff(cl); // this staff will be deleted and emited clef will set
						} else
								return; // when user selected piano staff again - do nothing
					} else // ordinary single staff
						if (cl.type() != Tclef::e_pianoStaff) { // simply set another clef
							if (cl.type() != m_clef.type()) {
								setClef(cl);
								emit clefChanged();
							}
						} else // demand to change it on piano staff
								emit switchPianoStaff(Tclef(Tclef::e_pianoStaff));
				}
			}
}


void TscoreClef::clefMenuStatusTip(QString tip) {
		emit statusTip(tip);
}


//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

void TscoreClef::getStatusTip() {
	QString tip = "<b>" + m_clef.name() + "</b>  (" + m_clef.desc() + ")";
	if (!readOnly())
		tip += "<br>" + tr("Click to select another clef.");
	setStatusTip(tip);
}



int TscoreClef::getYclefPos(Tclef clef) {
  int pos = 0;
  if (clef.type() == Tclef::e_treble_G || clef.type() == Tclef::e_treble_G_8down)
    pos = 11;
  else if (clef.type() == Tclef::e_bass_F|| clef.type() == Tclef::e_bass_F_8down)
    pos = 11;
  else if (clef.type() == Tclef::e_alto_C)
    pos = 11;
  else if (clef.type() == Tclef::e_tenor_C)
    pos = 9;
  return pos;
}


int TscoreClef::getClefPosInList(Tclef clef) {
  for (int i = 0; i < m_typesList.size(); i++)
    if (m_typesList[i] == clef.type()) {
      return i;
    }
    qDebug() << "getClefPosInList(): no clef was found";
		return 0;    
}




