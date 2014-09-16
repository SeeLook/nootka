/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TPATH_H
#define TPATH_H

#include "nootkacoreglobal.h"
#include <QString>

/** 
 * This is default path in Nootka. It is set at the beginning 
*/
class NOOTKACORE_EXPORT Tpath
{
	
public:
	
	static QString main; /** Path with Nootka resources (/usr/share -Linux /Resources - MacOs) */
	
			/** Returns path to Nootka images (picts) with given image name. 
			 *By default a '.png' extensions is added
			 * but it can be changed through @p ext parameter. */
	static QString img(const QString& image, const QString& ext = ".png") {	return main + "picts/" + image + ext;	}
	
			/** Returns a path to given ogg file with samples in sound resource directory */
	static QString sound(const QString& snd, const QString& ext = ".ogg") {	return main + "sounds/" + snd + ext;	}
};



#endif // TPATH_H
