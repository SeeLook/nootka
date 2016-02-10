/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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


#include <QtCore//qcoreapplication.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdatastream.h>


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  if (argc < 2) {
    qDebug() << "Usage:\nnootoxml file.noo file.xml";
    return 0;
  }

  QFile noo(a.arguments()[1]);
  if (noo.exists() && noo.open(QIODevice::ReadOnly)) {
    QDataStream in(&noo);
    quint32 examVersion;
    in >> examVersion;
    if (examVersion >= 0x95121708) {
      QByteArray arrayXML = noo.readAll();
      arrayXML.remove(0, 4);
      QByteArray unZipXml = qUncompress(arrayXML);
      if (!unZipXml.isEmpty()) {
        QFile xml(a.arguments()[2]);
        if (xml.open(QIODevice::WriteOnly)) {
          xml.write(unZipXml.data(), unZipXml.size());
          xml.close();
        } else
          qDebug() << "Can not write to" << xml.fileName();
      } else
        qDebug() << "Can not decompress file" << noo.fileName();
    } else
      qDebug() << noo.fileName() << "is not Nootka exam file!";
    noo.close();
  } else
    qDebug() << "Can not read from" << noo.fileName();


  return 0;
}
