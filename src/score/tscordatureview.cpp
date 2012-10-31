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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tscordatureview.h"
#include "ttune.h"
#include <QGraphicsTextItem>


TscordatureView::TscordatureView(QWidget* parent) :
  QGraphicsView(parent),
  m_hasScord(false),
  m_text(0)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet(("background: transparent"));
  setRenderHint(QPainter::TextAntialiasing, true);
  m_scene = new QGraphicsScene();
  setScene(m_scene);
}

void TscordatureView::setTune(Ttune tune, Tnote::EnameStyle nameStyle) {
  if (tune != Ttune::stdTune) {
    m_hasScord = true;
//     int fSize = height() / 3 - 2;
    int fSize = fontMetrics().boundingRect("A").height() - 2;
    int nL = 0;
    for (int i = 1; i < 7; i++) {
        if ( tune[i] != Ttune::stdTune[i])
            nL++;
    }
    QString scordText = "";
    int c = 0;
    for (int i = 1; i < 7; i++) {
        if ( tune[i] != Ttune::stdTune[i]) {
          if (c) { // first changed string was already writen
              if (nL > 3) { 
                if (c % 2 == 0) // add line break to second text in a row
                  scordText += "<br>";
                else // after first text space
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
    if (m_text)
        delete m_text;
    m_text = new QGraphicsTextItem();
    m_scene->addItem(m_text);
    m_text->setPos(0, 0);
    m_text->setHtml(scordText);
    resizeEvent(0);
  } else {
    m_hasScord = false;
    if (m_text) {
      delete m_text;
      m_text = 0;
    }
  }
}



TscordatureView::~TscordatureView() {}



void TscordatureView::resizeEvent(QResizeEvent*) {
  if (m_text) {
    if (m_text->boundingRect().height() != height()) // adjust by height
      m_text->setScale(height() / m_text->boundingRect().height());
    qreal sc = width() / m_text->boundingRect().width();
    if (sc < 1.0) // when too wide
      m_text->setScale(sc);
  }
}


