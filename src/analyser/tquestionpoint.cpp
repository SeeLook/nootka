/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                          	   	   *
 *   tomaszbojczuk@gmail.com   						                       *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "tquestionpoint.h"

/* static */
void TquestionPoint::setColors(QColor goodColor, QColor wrongColor, QColor notBadColor) {
    m_goodColor = goodColor;
    m_wrongColor = wrongColor;
    m_notBadColor = notBadColor;
}



TquestionPoint::TquestionPoint(TQAunit *question) :
    m_question(question)
{
    
}
  
void TquestionPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {
  
}
  
QRectF TquestionPoint::boundingRect() const {

}
  
  
private:
  TQAunit *m_question;
  
  static QColor m_goodColor, m_wrongColor, m_notBadColor;
  
};