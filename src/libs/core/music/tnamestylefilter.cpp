/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
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

#include "tnamestylefilter.h"

bool* TnameStyleFilter::m_is7th_B = 0;
Tnote::EnameStyle* TnameStyleFilter::m_solfegeStyle = 0;


TnameStyleFilter::TnameStyleFilter()
{}

void TnameStyleFilter::setStyleFilter(bool* is7th_B, Tnote::EnameStyle* solfegeStyle) {
    m_is7th_B = is7th_B;
    m_solfegeStyle = solfegeStyle;
}


Tnote::EnameStyle TnameStyleFilter::get(Tnote::EnameStyle style) {
  if (m_is7th_B) {
    if (style == Tnote::e_italiano_Si || style == Tnote::e_russian_Ci)
      return *(m_solfegeStyle);

    if (style == Tnote::e_deutsch_His || style == Tnote::e_nederl_Bis) {
      if (*(m_is7th_B))
        return Tnote::e_nederl_Bis;
      else
        return Tnote::e_deutsch_His;
    }

    if (style == Tnote::e_english_Bb|| style == Tnote::e_norsk_Hb) {
      if (*(m_is7th_B))
        return Tnote::e_english_Bb;
      else
        return Tnote::e_norsk_Hb;
    }
  }

  return style;
}



