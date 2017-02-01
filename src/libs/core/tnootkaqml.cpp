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

#include "tnootkaqml.h"
#include "ttickcolors.h"
#include "nootkaconfig.h"
#include "tpath.h"
#include "music/tmeter.h"
#include "music/tclef.h"
#include "music/tkeysignature.h"
#include "score/tscoreobject.h"
#include "score/tstaffobject.h"
#include <QtQml/qqmlengine.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>


TnootkaQML* TnootkaQML::m_instance = nullptr;


TnootkaQML::TnootkaQML(QObject* parent) :
  QObject(parent)
{
  if (m_instance) {
    qDebug() << "TnootkaQML instance already exists!";
    return;
  }
  m_instance = this;

  qRegisterMetaType<Tclef>();
  qmlRegisterUncreatableType<Tclef>("Score", 1, 0, "Tclef", "You cannot create an instance of the Tclef.");
  qRegisterMetaType<Tmeter>();
  qmlRegisterUncreatableType<Tmeter>("Score", 1, 0, "Tmeter", "You cannot create an instance of the Tmeter.");
  qmlRegisterUncreatableType<Trhythm>("Score", 1, 0, "Trhythm", "You cannot create an instance of the Trhythm.");

  qmlRegisterType<TscoreObject>("Score", 1, 0, "TscoreObject");
  qmlRegisterType<TstaffObject>("Score", 1, 0, "TstaffObject");

  qmlRegisterType<TtickColors>("TtickColors", 1, 0, "TtickColors");
}


TnootkaQML::~TnootkaQML()
{
  m_instance = nullptr;
}

//#################################################################################################
//###################       INVOKABLE METHODS          ############################################
//#################################################################################################

QString TnootkaQML::version() { return NOOTKA_VERSION; }


Tclef TnootkaQML::clef(int type) {
  return Tclef(static_cast<Tclef::EclefType>(type));
}


Tmeter TnootkaQML::meter(int m) {
  return Tmeter(static_cast<Tmeter::Emeter>(m));
}


Tnote TnootkaQML::note(int pitch, int octave, int alter, int rhythm, bool rest, bool dot) {
  return Tnote(static_cast<char>(pitch), static_cast<char>(octave), static_cast<char>(alter),
               Trhythm(static_cast<Trhythm::Erhythm>(rhythm), rest, dot, false));
}


QString TnootkaQML::majorKeyName(int key) {
  return TkeySignature(static_cast<char>(key)).getMajorName();
}


QString TnootkaQML::minorKeyName(int key) {
  return TkeySignature(static_cast<char>(key)).getMinorName();
}


QString TnootkaQML::getLicense() {
  QFile file(Tpath::main + QLatin1String("gpl"));
  QString license;
  QTextStream in;
  if (!file.exists()) { // Debian based
      QDir d(Tpath::main);
      d.cdUp();
      file.setFileName(d.path() + QLatin1String("/doc/nootka/copyright"));
  }
  if(file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&file);
      in.setCodec("UTF-8");
      license = in.readAll();
  }
  file.close();
  return license;
}


QString TnootkaQML::getChanges() {
  QFile file(Tpath::main + QLatin1String("changes"));
  QString chLog;
  QTextStream in;
  if(file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&file);
      in.setCodec("UTF-8");
      QStringList htmlText = in.readAll().replace(QLatin1String("  "), QLatin1String("&nbsp;&nbsp;")).split(QLatin1String("\n"));
      for (int i = 0; i < htmlText.size(); i++) {
        if (htmlText[i].startsWith(QLatin1String("0.")) || htmlText[i].startsWith(QLatin1String("1.")))
          htmlText[i] = QLatin1String("<span style=\"font-size: x-large; color: #0000ff\"><b>&nbsp;") + htmlText[i] + QLatin1String("</b></span>");
        else if (htmlText[i].contains(QLatin1String("======")))
          htmlText[i] = QStringLiteral("<br><hr><b><big><center>Nootka ONE</big></b></center><hr>");
        else if (htmlText[i].contains(QLatin1String("BUG")))
          htmlText[i] = QStringLiteral("&nbsp;&nbsp;<u>BUG FIXES</u>");
        else if (htmlText[i].contains(QLatin1String("Under the hood")))
          htmlText[i] = QStringLiteral("&nbsp;&nbsp;<u>Under the hood</u>");
        else if (!htmlText[i].contains(QLatin1String("&nbsp;&nbsp; - ")))
          htmlText[i] = QLatin1String("<b>") + htmlText[i] + QLatin1String("</b>");
        htmlText[i].append(QLatin1String("<br>"));
      }
      chLog = htmlText.join(QString());
  }
  file.close();
  return chLog;
}





