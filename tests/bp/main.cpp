/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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


#include <QtCore//qcoreapplication.h>
#include <QtCore/qfile.h>
#include <QtCore/qxmlstream.h>
#include <QtCore/qdebug.h>
#include <music/tnote.h>


struct Butt {
  double cx;
  double cy;
  QString hi;
  QString lo;
  short chHi;
  short chLo;
};


short chromatic(const QString& s) {
  QString step = s.left(1).toUpper();
  char note = 0, alter = 0, octave = 0;
  for (char i = 1; i < 8; i++) {
    Tnote n(i, 0, 0);
    if (n.toText(Tnote::e_english_Bb, false) == step) {
      note = i;
      break;
    }
  }
//   qDebug() << s.mid(1, 1).data();
  if (s.mid(1, 1) == QString::fromUtf8("♭"))
    alter = -1;
  octave = static_cast<char>(s.right(1).toInt() - 3);
  return Tnote(note, octave, alter).chromatic();
}

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  if (argc < 2) {
    qDebug() << "Usage:\nbp file.svg";
    return 0;
  }

  double svgHeight = 0.0, minX = 1000.0, minY = 1000.0, maxY = 0.0, maxX = 0.0;
  QFile svg(a.arguments()[1]);
  if (svg.exists() && svg.open(QIODevice::ReadOnly)) {
    QXmlStreamReader xml(&svg);
    if (xml.readNextStartElement()) {
      if (xml.name() != QLatin1String("svg")) {
          qDebug() << svg.fileName() << "is not svg type";
      } else {
          svgHeight = xml.attributes().value(QStringLiteral("height")).toDouble();
          while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("g")) {
              QList<Butt> bList;
              QList<Butt> tList;
              int bId = 0;
              while (xml.readNextStartElement()) {
                if (xml.name() == QLatin1String("circle")) {
                    Butt b;
                    b.cx = xml.attributes().value(QStringLiteral("cx")).toDouble();
                    minX = qMin(b.cx, minX);
                    maxX = qMax(b.cx, maxX);
                    b.cy = xml.attributes().value(QStringLiteral("cy")).toDouble();
                    maxY = qMax(b.cy, maxY);
                    minY = qMin(b.cy, minY);
                    bList << b;
                    xml.skipCurrentElement();
                } else if (xml.name() == QLatin1String("text")) {
                    bool isHi = true;
                    Butt t;
                    t.cx = xml.attributes().value(QStringLiteral("x")).toDouble();
                    t.cy = xml.attributes().value(QStringLiteral("y")).toDouble();
                    while (xml.readNextStartElement()) {
                      if (isHi) {
                          t.chHi = chromatic(xml.readElementText(QXmlStreamReader::IncludeChildElements));
                          isHi = false;
                      } else
                          t.chLo = chromatic(xml.readElementText(QXmlStreamReader::IncludeChildElements));
                    }
                    ++bId;
                    tList << t;
                } else
                    xml.skipCurrentElement();
              }
              for (int i = 0; i < bList.size(); ++i) {
                Butt& b = bList[i];
                for (int j = 0; j < tList.size(); ++j) {
                  Butt& t = tList[j];
                  if (t.cx - 2.0 < b.cx && t.cx + 2.0 > b.cx && t.cy - 2.0 < b.cy && t.cy + 2.0 > b.cy) {
                    b.chHi = t.chHi; b.chLo = t.chLo;
                    b.cx -= minX;
                    b.cy = svgHeight - (maxY - b.cy);
                    double x = b.cx, y = b.cy;
//                     b.cx = -y + maxY; b.cy = x; // left
                    b.cx = y; b.cy = -x + minX - svgHeight; // right
                    break;
                  }
                }
                QString out = QString("%1, %2, %3, %4").arg(b.cx).arg(b.cy).arg(b.chHi).arg(b.chLo);
                qDebug() << "{" << out << "}, //" << Tnote(b.chHi).toText() << Tnote(b.chLo).toText();
              }
            }
            xml.skipCurrentElement();
        }
      }
    }
    svg.close();
  } else
      qDebug() << "Can not read from" << svg.fileName();
  qDebug() << "minX" << minX << "maxX" << maxX << "minY" << minY <<  "maxY" << maxY << "h:" << svgHeight;

  return 0;
}
