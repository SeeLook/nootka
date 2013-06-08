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
#include "ttune.h"
#include <QGraphicsView>

TscoreScordature::TscoreScordature(TscoreScene* scene) :
  TscoreItem(scene),
  m_scordText(0)
{
  
  setStatusTip(tr("Scordature of a guitar"));
}


void TscoreScordature::setTune(Ttune& tune, Tnote::EnameStyle nameStyle) {
  if (tune != Ttune::stdTune) {
//     m_hasScord = true;
//     int fSize = height() / 3 - 2;
    int fSize = scoreScene()->views()[0]->fontMetrics().boundingRect("A").height() - 2;
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
//     m_scordText->setPos(0, 0);
  } else {
//     m_hasScord = false;
    if (m_scordText) {
      delete m_scordText;
      m_scordText = 0;
    }
  }
}


QRectF TscoreScordature::boundingRect() {
  if (m_scordText)
      return m_scordText->boundingRect();
  else
    return QRectF(0, 0, 1, 1);
}


