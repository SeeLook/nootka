/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk & José Luis Marín                *
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

#ifndef TZIP_H
#define TZIP_H

#include "nootkacoreglobal.h"
#include <QtCore/qstring.h>

// class zipFile;

/**
 * Simple C++ proxy for @p zlib and minizip.
 */
class NOOTKACORE_EXPORT Tzip
{
public:
    /**
     * Reads music XML data from @p zipFile (full path)
     * into @p xmlData @p QByteArray
     */
    static void getXmlFromZip(const QString &zipFile, QByteArray *xmlData);

    /**
     * Creates *,mxl file (compressed musicXML) with @p xmlData.
     * Returns @p TRUE when success.
     */
    static bool zipMusicXml(const QString &zipFile, QByteArray *xmlData);

private:
    /**
     * Common routine that zips @p buff under @p zFilename name
     * inside @p zFile zip file.
     * If @p zFilename contains '/' i.e. 'META-INF/container.xml'
     * file 'container.xml' is created inside 'META-INF' directory of the zip file.
     */
    static bool writeBuff(void *zFile, const char *zFilename, const char *buff, size_t buffsize);
};

#endif // TZIP_H
