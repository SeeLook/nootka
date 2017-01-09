/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#ifndef TNAMESTYLEFILTER_H
#define TNAMESTYLEFILTER_H


#include "tnote.h"

/** This is very simple class, more to describe than to do.
 * This class converts given style according to global Nootka settings.
 * It gives possibility to have name style always matched with user preferences
 * even if exam file was generated with other styles.
 * In fact, there are three possible name styles:
 * - solfege Do Re ...
 * - letter with sign C# Db
 * - letter full name Cis Des
 * All that e_italiano_Si or e_english_Bb depends on:
 * Tnote::EnameStyle S->solfegeStyle
 * bool S->seventhIs_B
 * declared in Tglobals.
 * get(Tnote::EnameStyle style) static method returns proper style.
 */
class NOOTKACORE_EXPORT TnameStyleFilter
{

public:
    TnameStyleFilter();

        /** Initialize filter with pointers to global settings */
    static void setStyleFilter(bool *is7th_B, Tnote::EnameStyle *solfegeStyle);
        /** Perform filtering */
    static Tnote::EnameStyle get(Tnote::EnameStyle style);

        /** Usually it is pointer to global is7th_B.  */
    static bool* is7th_B() { return m_is7th_B; }
        /** Usually it is pointer to global solfegeStyle.  */
    static Tnote::EnameStyle* solfegeStyle() { return m_solfegeStyle; }


private:
    static bool                   *m_is7th_B;
    static Tnote::EnameStyle       *m_solfegeStyle;

};

#endif // TNAMESTYLEFILTER_H
