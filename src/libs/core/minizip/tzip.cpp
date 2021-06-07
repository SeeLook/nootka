/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#include "tzip.h"
#include "zip.h"
#include "unzip.h"

#include <QtCore/qfileinfo.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdebug.h>

#define BUFF_SIZE (8192)


void Tzip::getXmlFromZip(const QString &zipFile, QByteArray* xmlData) {
  auto mxlFile = unzOpen64(zipFile.toStdString().data());

  if (mxlFile != nullptr) {
    char*               name = new char[255];
    unz_file_info64     finfo;
    int                 ret;

    do {
      ret = unzGetCurrentFileInfo64(mxlFile, &finfo, name, 255, nullptr, 0, nullptr, 0);
      if (ret == UNZ_OK) {
        QString qName(name);
        char*             buff = new char[BUFF_SIZE];
        int               red;
        int               unRet;
        std::string       container;
//         qDebug() << "name" << qName << "utf8" << ((finfo.flag & (1 << 11)) ? true : false);
        if (qName.endsWith(QLatin1String("container.xml"))) {
          // TODO verify is it true:
          // We might read file name with music data from container.xmlData
          // but it seems there is only single *.xml file in zip root.
          // Another xml is container.xml itself but inside META_INF folder,
          // so next if statement should be sufficient.
          // Then delete following commented code for good.
//             unRet = unzOpenCurrentFile(mxlFile);
//             while ((red = unzReadCurrentFile(mxlFile, buff, BUFF_SIZE)) > 0) {
//               container.append(buff, red);
//             }
//             if (red < 0)
//               unzCloseCurrentFile(mxlFile);
        } else if (qName.endsWith(QLatin1String(".xml")) && !qName.startsWith(QLatin1String("META-INF"))) {
            // xml file which is not container is rather musicXML
            unRet = unzOpenCurrentFile(mxlFile);
            while ((red = unzReadCurrentFile(mxlFile, buff, BUFF_SIZE)) > 0) {
              xmlData->append(buff, red);
            }
            if (red < 0)
              unzCloseCurrentFile(mxlFile);
        }
        delete[] buff;
      }
      ret = unzGoToNextFile(mxlFile);
    } while (ret == UNZ_OK);

    delete[] name;
  }

  unzClose(mxlFile);
}


bool Tzip::zipMusicXml(const QString &zipFile, QByteArray *xmlData) {
  auto zFile = zipOpen64(zipFile.toLocal8Bit().data(), 0);

  if (zFile == NULL)
    return false;

  // Compressed musicXML file contains three entries:
  // 1. mimetype
  std::string buf = "application/vnd.recordare.musicxml\n";
  if (!writeBuff(zFile, "mimetype", buf.data(), buf.length()))
    return false;

  // 2. META-INF folder with container.xml where is xml file name.
  std::string filename = QFileInfo(zipFile).baseName().toStdString() + ".xml";
  buf = std::string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<container>\n<rootfiles>\n")
      + "<rootfile full-path=\"" + filename + "\" media-type=\"application/vnd.recordare.musicxml+xml\">\n"
      + "</rootfile>\n</rootfiles>\n</container>";
  if (!writeBuff(zFile, "META-INF/container.xml", buf.data(), buf.length()))
    return false;

  // 3. musicXML file itself
  if (!writeBuff(zFile, filename.data(), xmlData->data(), xmlData->size()))
    return false;

  zipClose(zFile, NULL);

  return true;
}


//#################################################################################################
//###################                PRIVATE           ############################################
//#################################################################################################

bool Tzip::writeBuff(void* zFile, const char* zfilename, const char* buff, size_t buffsize) {
  auto zf = reinterpret_cast<zipFile>(zFile);

  if (zf == NULL)
    return false;

  int ret;
  ret = zipOpenNewFileInZip64(zf, zfilename, NULL, NULL, 0, NULL, 0, NULL,
                              Z_DEFLATED, Z_DEFAULT_COMPRESSION, 1);
  if (ret != ZIP_OK)
    return false;

  ret = zipWriteInFileInZip(zf, buff, buffsize);
  zipCloseFileInZip(zf);

  return ret == ZIP_OK ? true : false;
}
