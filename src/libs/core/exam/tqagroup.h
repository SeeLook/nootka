/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TQAGROUP_H
#define TQAGROUP_H


#include <nootkacoreglobal.h>
#include <tfingerpos.h>
#include <music/tnote.h>


/** 
 * This is simple structure with 
 * @p note (Tnote) 
 * and position on the guitar @p pot (TfingerPos)
 */
struct NOOTKACORE_EXPORT TQAgroup 
{
		TfingerPos pos;
		Tnote note;
};

		/** Global method to save TQAgroup to XML. (not valid) note and position are skipped. */
NOOTKACORE_EXPORT void qaGroupToXml(TQAgroup& gr, QXmlStreamWriter& xml, const QString& tag = "qa");

		/** Global method to get TQAgroup from XML. */
NOOTKACORE_EXPORT bool qaGroupFromXml(TQAgroup& gr, QXmlStreamReader& xml);


#endif // TQAGROUP_H