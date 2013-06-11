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

#include "tscorescordature.h"
#include "tscorescene.h"
#include "tscorestaff.h"
#include "tscoreclef.h"
#include "ttune.h"
#include <QGraphicsView>

#include <QDebug>

/*static*/
Tnote::EnameStyle TscoreScordature::nameStyle = Tnote::e_nederl_Bis;



TscoreScordature::TscoreScordature(TscoreScene* scene, TscoreStaff* staff) :
  TscoreItem(scene),
  m_scordText(0),
  m_height(1)
{
  setStaff(staff);
  setStatusTip(tr("Scordature of a guitar"));
}


void TscoreScordature::setTune(Ttune& tune) {
  if (tune != Ttune::stdTune) {
		int fSize = 12;
    int nL = 0;
    for (int i = 1; i < 7; i++) {
        if ( tune[i] != Ttune::stdTune[i])
            nL++;
    }
    QString scordText = "";
    int c = 0;
    for (int i = 1; i < 7; i++) {
        if ( tune[i] != Ttune::stdTune[i]) {
          if (c) { // string  changed first was already writen
              if (nL > 3) { 
                if (c % 2 == 0) // add line break to second entry in a row
                  scordText += "<br>";
                else // space after first text entry
                  scordText += " ";              
              } else // single text in row, so line break after
                scordText += "<br>";
          }
          scordText += QString("<span style=\"font-family: nootka; font-size: %1px;\">%2=</span>").arg(fSize).arg(i);
          scordText += QString("<span style=\"font-size: %1px;\">%2</span>").arg(fSize).
              arg(tune[i].toText(nameStyle, false));
          c++;
        }
    }
    if (m_scordText)
        delete m_scordText;
    m_scordText = new QGraphicsTextItem();
    m_scordText->setParentItem(this);
		m_scordText->setHtml(scordText);		
		qreal xPos = 1.0, extraW = 0.0;
		qreal yPos = staff()->upperLinePos() + 11.5;
		if (staff()->kindOfStaff() == TscoreStaff::e_lower) {
			// This is in case of scordature on piano staff - it needs more space to look well
			xPos = -1.0;
			extraW = 2.0;
			yPos = staff()->upperLinePos() + 7.5;
		}
		qreal factor = (CLEF_WIDTH + KEY_WIDTH / 2 + extraW) / m_scordText->boundingRect().width();
		setScale(factor);
		m_height = staff()->height() - yPos;
// 		qDebug() << m_height << m_scordText->boundingRect().height() * scale();
		if (m_scordText->boundingRect().height() * scale() > m_height)
			setScale(factor * (m_height / (m_scordText->boundingRect().height() * scale())));
		setPos(xPos, yPos);
  } else {
    if (m_scordText) {
      delete m_scordText;
      m_scordText = 0;
    }
  }
}


QRectF TscoreScordature::boundingRect() const {
  if (m_scordText)
		return QRectF(0, 0, (CLEF_WIDTH + KEY_WIDTH - 1) / scale(), m_height / scale());
}


void TscoreScordature::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
// 	painter->drawRect(boundingRect());
}


