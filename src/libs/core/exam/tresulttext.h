/***************************************************************************
 *   Copyright (C) 2015-2018 by Tomasz Bojczuk                             *
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


#ifndef TRESULTTEXT_H
#define TRESULTTEXT_H

#include <nootkacoreglobal.h>
#include <QtCore/qstring.h>

class TQAunit;
class QColor;

    /**
     * Returns HTML formatted text with answer details.
     * If @p fontSize remains default - default @p fontSize is taken.
     * When @p attempt is bigger than 0 (and answer was a melody of course)
     * The summary of that attempt is prepared.
     */
NOOTKACORE_EXPORT QString wasAnswerOKtext(TQAunit* answer, int attempt = 0);

#endif // TRESULTTEXT_H
