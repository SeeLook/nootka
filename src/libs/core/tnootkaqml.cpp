/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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
#include "qtr.h"
#include "nootkaconfig.h"
#include "tglobals.h"
#include "tpath.h"
#include "music/tkeysignature.h"
#include "music/tnamestylefilter.h"
#include "score/tnotepair.h"
#include "score/tscoreobject.h"
#include "score/tstaffitem.h"
#include "score/tnoteitem.h"
#include "score/tstafflines.h"
#include "score/taddnoteitem.h"
#include "instruments/tguitarbg.h"
#include "instruments/tpianobg.h"
#include "instruments/tbandoneonbg.h"
#include "instruments/tsaxbg.h"
#include "taction.h"
#include "music/ttuneobject.h"
#include "tmtr.h"
#include "tcolor.h"

#include <QtQml/qqmlengine.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <QtCore/qdir.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qbuffer.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qdesktopservices.h>
#if defined (Q_OS_ANDROID)
  #include "Android/tfiledialog.h"
  #include "Android/tmobilemenu.h"
#else
  #include <QtWidgets/qfiledialog.h>
#endif

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

  qsrand(QDateTime::currentDateTime().toTime_t());

  qRegisterMetaType<Tclef>();
  qmlRegisterUncreatableType<Tclef>("Score", 1, 0, "Tclef", QStringLiteral("You cannot create an instance of the Tclef."));
  qRegisterMetaType<Tmeter>();
  qmlRegisterUncreatableType<Tmeter>("Score", 1, 0, "Tmeter", QStringLiteral("You cannot create an instance of the Tmeter."));
  qmlRegisterUncreatableType<Trhythm>("Score", 1, 0, "Trhythm", QStringLiteral("You cannot create an instance of the Trhythm."));
  qRegisterMetaType<Trhythm>();

  qmlRegisterType<TscoreObject>("Score", 1, 0, "TscoreObject");
  qmlRegisterType<TstaffItem>("Score", 1, 0, "TstaffItem");
  qmlRegisterType<TnoteItem>("Score", 1, 0, "TnoteItem");
  qmlRegisterType<TstaffLines>("Score", 1, 0, "TstaffLines");
  qmlRegisterType<TaddNoteItem>("Score", 1, 0, "TaddNoteItem");

  qmlRegisterUncreatableType<TcommonInstrument>("Nootka", 1, 0, "TcommonInstrument", QStringLiteral("You cannot create an instance of the TcommonInstrument."));
  qmlRegisterType<TguitarBg>("Nootka", 1, 0, "TguitarBg");
  qmlRegisterType<TpianoBg>("Nootka", 1, 0, "TpianoBg");
  qmlRegisterType<TbandoneonBg>("Nootka", 1, 0, "TbandoneonBg");
  qmlRegisterType<TsaxBg>("Nootka", 1, 0, "TsaxBg");
  qmlRegisterType<Taction>("Nootka", 1, 0, "Taction");
#if defined (Q_OS_ANDROID)
  qmlRegisterType<TmobileMenu>("Nootka", 1, 0, "TmobileMenu");
#endif

  qmlRegisterUncreatableType<TnootkaQML>("Nootka", 1, 0, "Nootka", QStringLiteral("You cannot create an instance of the TnootkaQML."));
  qRegisterMetaType<Tinstrument>();
  qmlRegisterUncreatableType<Tinstrument>("Nootka", 1, 0, "Tinstrument", QStringLiteral("You cannot create an instance of the Tinstrument."));
  qRegisterMetaType<Ttune>();
  qmlRegisterUncreatableType<Ttune>("Nootka", 1, 0, "Ttune", QStringLiteral("You cannot create an instance of the Ttune."));
  qmlRegisterType<TtuneObject>("Nootka", 1, 0, "TtuneObject");
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


Tnote TnootkaQML::note(const Tnote& n, int rhythm, bool rest, bool dot) {
  return Tnote(n, Trhythm(static_cast<Trhythm::Erhythm>(rhythm), rest, dot, false));
}


Tnote TnootkaQML::note(const Tnote& n, const Trhythm& r) {
  return Tnote(n, r);
}


Tnote TnootkaQML::note(int chroma,  bool sharp) {
  Tnote n(static_cast<short>(chroma));
  if (!sharp && (n.alter() != 0 || (n.alter() == 0 && n.note() != 3 && n.note() != 7))) // but skip e and b - otherwise they become fb or cb
    n = n.showWithFlat();
  return n;
}


Tnote TnootkaQML::transpose(Tnote n, int semitones) {
  n.transpose(semitones);
  return n;
}



Trhythm TnootkaQML::rhythm(int rtm, bool rest, bool dot, bool triplet) {
  return Trhythm(static_cast<Trhythm::Erhythm>(rtm), rest, dot, triplet);
}


/**
 * It returns glyphs of 'Nootka' font in contrary to TnoteObject::getHeadText()
 */
QString TnootkaQML::rhythmText(const Trhythm& r) {
  if (r.rhythm() == Trhythm::NoRhythm)
    return QStringLiteral("z"); // just black note-head
  QString out;
  if (r.isRest())
    out = QString(QChar(0xe106 + static_cast<int>(r.rhythm())));
  else
    out = QString(QChar(66 + static_cast<int>(r.rhythm())));
  if (r.hasDot())
    out += QStringLiteral(".");
  return out;
}


QString TnootkaQML::noteName(const Tnote& n, int style, bool showOctave) {
  // Tnote::toText() method returns only names in user preferred according to settings
  // To cheat it and force note name in any given style we are resetting pointer of is7th_B 
  // then Tnote skips filtering of a style during name generation.
  auto tmpPtr = TnameStyleFilter::is7th_B();
  TnameStyleFilter::setStyleFilter(nullptr, TnameStyleFilter::solfegeStyle());
  auto name = n.toText(static_cast<Tnote::EnameStyle>(style), showOctave);
  TnameStyleFilter::setStyleFilter(tmpPtr, TnameStyleFilter::solfegeStyle()); // restore is7th_B settings
  return name;
}


QString TnootkaQML::majorKeyName(int key) {
  return TkeySignature(static_cast<char>(key)).getMajorName();
}


QString TnootkaQML::minorKeyName(int key) {
  return TkeySignature(static_cast<char>(key)).getMinorName();
}


QString TnootkaQML::majAndMinKeyName(int key) {
  return majorKeyName(key) + QLatin1String("<br>") + minorKeyName(key);
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


bool TnootkaQML::isAndroid()  {
#if defined (Q_OS_ANDROID)
  return true;
#else
  return false;
#endif
}


QStringList TnootkaQML::guitarTunings() {
  QStringList tunList;
  int start = static_cast<int>(Ttune::Standard_EADGBE);
  for (int t = start; t < start + 5; ++t)
    tunList << Ttune::definedName(static_cast<Ttune::Etunings>(t));
  tunList << QApplication::translate("InstrumentPage", "Custom tuning");
  return tunList;
}


QStringList TnootkaQML::bassTunings() {
  QStringList tunList;
  int start = static_cast<int>(Ttune::Bass4_EADG);
  for (int t = start; t < start + 4; ++t)
    tunList << Ttune::definedName(static_cast<Ttune::Etunings>(t));
  tunList << QApplication::translate("InstrumentPage", "Custom tuning");
  return tunList;
}


Ttune TnootkaQML::tuning(int tuningType) {
  if (tuningType > -1) {
    if (tuningType == 0)
      return Ttune::stdTune;
    if (tuningType < 5)
      return Ttune::tunes[tuningType - 1];
    if (tuningType > 99 && tuningType < 104)
      return Ttune::bassTunes[tuningType - 100];
  }
  return Ttune();
}


/**
 * When third note @s3 is valid the tuning represents real guitar tuning
 * otherwise it is an instrument scale
 */
Ttune TnootkaQML::tuning(const Tnote& s1, const Tnote& s2, const Tnote& s3, const Tnote& s4, const Tnote& s5, const Tnote& s6) {
  return Ttune(QApplication::translate("InstrumentPage", "Custom tuning"), s1, s2, s3, s4, s5, s6, s3.isValid() ? Ttune::Custom : Ttune::Scale);
}


Tinstrument TnootkaQML::instr(int type) {
  return Tinstrument(static_cast<Tinstrument::Etype>(type < 0 || type > INSTR_COUNT - 1 ? 0 : type));
}


QString TnootkaQML::getXmlToOpen() {
  QString openFile;
#if defined (Q_OS_ANDROID)
  openFile = TfileDialog::getOpenFileName(nullptr, GLOB->lastXmlDir(), QStringLiteral("xml"));
#else
  openFile = QFileDialog::getOpenFileName(nullptr, qApp->translate("TmelMan", "Open melody file"), GLOB->lastXmlDir(),
                                      qApp->translate("TmelMan", "MusicXML file") + QLatin1String(" (*.xml)"));
#endif
  if (!openFile.isEmpty())
    GLOB->setLastXmlDir(QFileInfo(openFile).absoluteDir().path());
  return openFile;
}


QString TnootkaQML::getXmlToSave() {
  QString saveFile;
#if defined (Q_OS_ANDROID)
  saveFile = TfileDialog::getSaveFileName(nullptr, GLOB->lastXmlDir(), QStringLiteral("xml"));
#else
  saveFile = QFileDialog::getSaveFileName(nullptr, qApp->translate("TmelMan", "Save melody as:"), GLOB->lastXmlDir(),
                                      qTR("TmelMan", "MusicXML file") + QLatin1String(" (*.xml)"));
#endif
  if (!saveFile.isEmpty())
    GLOB->setLastXmlDir(QFileInfo(saveFile).absoluteDir().path());
  return saveFile;
}


QString TnootkaQML::aboutQt() {
  return "<h1 align=\"center\">"
  + qTR("QMessageBox", "<h3>About Qt</h3><p>This program uses Qt version %1.</p>").arg(qVersion())
        .replace(QLatin1String("<p>"), QString()).replace(QLatin1String("</p>"), QString()) + "</h1><br>"
  + qTR("QMessageBox",
      "<p>Qt is a C++ toolkit for cross-platform application "
      "development.</p>"
      "<p>Qt provides single-source portability across all major desktop "
      "operating systems. It is also available for embedded Linux and other "
      "embedded and mobile operating systems.</p>"
      "<p>Qt is available under three different licensing options designed "
      "to accommodate the needs of our various users.</p>"
      "<p>Qt licensed under our commercial license agreement is appropriate "
      "for development of proprietary/commercial software where you do not "
      "want to share any source code with third parties or otherwise cannot "
      "comply with the terms of the GNU LGPL version 3 or GNU LGPL version 2.1.</p>"
      "<p>Qt licensed under the GNU LGPL version 3 is appropriate for the "
      "development of Qt&nbsp;applications provided you can comply with the terms "
      "and conditions of the GNU LGPL version 3.</p>"
      "<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the "
      "development of Qt&nbsp;applications provided you can comply with the terms "
      "and conditions of the GNU LGPL version 2.1.</p>"
      "<p>Please see <a href=\"http://%2/\">%2</a> "
      "for an overview of Qt licensing.</p>"
      "<p>Copyright (C) %1 The Qt Company Ltd and other "
      "contributors.</p>"
      "<p>Qt and the Qt logo are trademarks of The Qt Company Ltd.</p>"
      "<p>Qt is The Qt Company Ltd product developed as an open source "
      "project. See <a href=\"http://%3/\">%3</a> for more information.</p>"
  ).arg(QStringLiteral("2017"),
        QStringLiteral("qt.io/licensing"),
        QStringLiteral("qt.io"));
}


QString TnootkaQML::pix(const QString& imageFileName) {
  return Tpath::pix(imageFileName);
}


QString TnootkaQML::TR(const QString& context, const QString& text, const QString& disambiguation, int n) {
  return qTR(qPrintable(context), qPrintable(text), qPrintable(disambiguation), n);
}


QColor TnootkaQML::alpha(const QColor& c, int a) {
  return Tcolor::alpha(c, a);
}


QColor TnootkaQML::randomColor(int alpha, int level) {
  return QColor(qrand() % level, qrand() % level, qrand() % level, alpha);
}


QColor TnootkaQML::invert(const QColor& c) {
  return Tcolor::invert(c);
}


int TnootkaQML::fontSize() {
#if defined (Q_OS_ANDROID)
  return (qApp->font().pixelSize() > 0 ? qApp->font().pixelSize() : qApp->font().pointSize()) * 0.8 * GLOB->guiScale();
#else
  return Tmtr::fingerPixels() * 0.45 * GLOB->guiScale();
#endif
}


QString TnootkaQML::pixToHtml(const QString& pixName, int height) {
  if (height == 0)
    return QString("<img src=\"%1\">").arg(pixName);

  QPixmap pix;
  if (!pix.load(Tpath::img(qPrintable(pixName))))
    return QString();

  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  pix.scaled(qRound(height * (static_cast<qreal>(pix.height()) / static_cast<qreal>(pix.width()))),
                               height, Qt::KeepAspectRatio, Qt::SmoothTransformation).save(&buffer, "PNG");
  return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
}


void TnootkaQML::openHelpLink(const QString& hash) {
  QDesktopServices::openUrl(QUrl(QString("https://nootka.sourceforge.io/index.php?L=%1&C=doc#" + hash).
    arg(QString(qgetenv("LANG")).left(2).toLower()), QUrl::TolerantMode));
}


qreal TnootkaQML::bound(qreal min, qreal val, qreal max) {
  return qBound(min, val, max);
}


//#################################################################################################
//###################     CONNECTIONS  NODE            ############################################
//#################################################################################################
void TnootkaQML::noteStarted(const Tnote& n) {
  Tnote transNote = n;
  if (transNote.isValid())
    transNote.transpose(-GLOB->transposition());
  if (m_scoreObject->keySignature() < 0 || (m_scoreObject->keySignature() == 0 && GLOB->GpreferFlats))
    transNote = transNote.showWithFlat();
  m_ignoreScore = true;
  if (m_scoreObject->singleNote()) {
      if (!transNote.isRest()) {
        transNote.setRhythm(Trhythm::NoRhythm);
        m_scoreObject->setNote(0, transNote);
      }
  } else
      m_scoreObject->addNote(transNote, true);
}


void TnootkaQML::noteFinished(const Tnote& n) {
  m_instrument->setNote(n);
  Tnote transNote = n;
  if (transNote.isValid())
    transNote.transpose(-GLOB->transposition());
  if (m_scoreObject->keySignature() < 0 || (m_scoreObject->keySignature() == 0 && GLOB->GpreferFlats))
    transNote = transNote.showWithFlat();
  m_ignoreScore = true;
  if (m_scoreObject->singleNote())
    transNote.setRhythm(Trhythm::NoRhythm);
  if (m_scoreObject->selectedItem() && (!m_scoreObject->singleNote() || (m_scoreObject->singleNote() && !transNote.isRest())))
    m_scoreObject->setNote(m_scoreObject->selectedItem(), transNote);
//   else // naughty user pressed arrow key
  // TODO remember to treat tied notes as a single one when setNote will be implemented
}


void TnootkaQML::setMainScore(QQuickItem* ms) {
  if (!m_mainScore) {
    m_mainScore = ms;
    m_scoreObject = qobject_cast<TscoreObject*>(qvariant_cast<QObject*>(m_mainScore->property("scoreObj")));
    connect(m_scoreObject, &TscoreObject::selectedNoteChanged, this, &TnootkaQML::scoreChangedNoteSlot);
    connect(GLOB, &Tglobals::isExamChanged, this, &TnootkaQML::examStartStop);
    if (m_scoreObject && !m_nodeConnected)
      connectInstrument();
  }
}


void TnootkaQML::setInstrument(TcommonInstrument* ci) {
  if (m_instrument != ci) {
    if (m_instrument != nullptr)
      m_nodeConnected = false; // reset connection of instrument signal when instrument type changed
      m_instrument = ci;
    if (m_scoreObject && !m_nodeConnected)
      connectInstrument();
  }
}


void TnootkaQML::connectInstrument() {
  m_nodeConnected = true;
  connect(m_instrument, &TcommonInstrument::noteChanged, this, &TnootkaQML::instrumentChangesNoteSlot);
}


void TnootkaQML::instrumentChangesNoteSlot() {
  Tnote rawNote = m_instrument->note();
  qDebug() << "instrument send note" << rawNote.toText();
  m_ignoreScore = true;
  emit playNote(m_instrument->note()); // not yet transposed - to sound properly
  rawNote.transpose(-GLOB->transposition());
  if (m_scoreObject->keySignature() < 0 || (m_scoreObject->keySignature() == 0 && GLOB->GpreferFlats))
    rawNote = rawNote.showWithFlat();

  if (m_scoreObject->singleNote()) {
      m_scoreObject->setNote(0, rawNote);
      m_scoreObject->setTechnical(0, m_instrument->technical());
  } else {
      if (m_scoreObject->selectedItem()) {
          rawNote.setRhythm(m_scoreObject->selectedItem()->note()->rtm);
          m_scoreObject->setNote(m_scoreObject->selectedItem(), rawNote);
      } else {
          rawNote.setRhythm(m_scoreObject->workRhythm());
          m_scoreObject->addNote(rawNote, true);
      }
      if (GLOB->instrument().type() == Tinstrument::Bandoneon) {
        auto seg = m_scoreObject->selectedItem() ? m_scoreObject->noteSegment(m_scoreObject->selectedItem()->index()) : m_scoreObject->lastSegment();
        Ttechnical instrData(m_instrument->technical());
        if (seg->index() > 0) {
          for (int i = seg->index() - 1; i >= 0; --i) {
            auto searchNoteData = m_scoreObject->noteSegment(i)->techicalData();
            if (searchNoteData.bowing()) { // Show bowing but only when it changes comparing to the previously  set bow direction
              if (searchNoteData.bowing() == instrData.bowing()) // if it is the same - just reset bowing on note data from the instrument
                instrData.setBowing(Ttechnical::BowUndefined);
              break;
            }
          }
        }
        seg->setTechnical(instrData.data());
      }
  }
}


void TnootkaQML::examStartStop() {
  if (GLOB->isExam()) {
      disconnect(m_instrument, &TcommonInstrument::noteChanged, this, &TnootkaQML::instrumentChangesNoteSlot);
      disconnect(m_scoreObject, &TscoreObject::selectedNoteChanged, this, &TnootkaQML::scoreChangedNoteSlot);
  } else {
      m_nodeConnected = false;
      connectInstrument();
      connect(m_scoreObject, &TscoreObject::selectedNoteChanged, this, &TnootkaQML::scoreChangedNoteSlot);
  }
}


void TnootkaQML::scoreChangedNoteSlot() {
  if (m_ignoreScore) {
    m_ignoreScore = false;
    return;
  }
  auto n = m_scoreObject->selectedNote();
  if (n.isValid())
    n.transpose(GLOB->transposition());
  m_instrument->setNote(n, getTechicalFromScore());
  emit playNote(n);
  qDebug() << "Got note from score" << n.toText() << n.chromatic();
}


int TnootkaQML::selectedNoteId() const {
  return m_scoreObject->selectedItem() ? m_scoreObject->selectedItem()->index() : -1;
}


int TnootkaQML::getTechicalFromScore() {
  quint32 technical = 255; // empty by default
  if (GLOB->instrument().type() == Tinstrument::Bandoneon && m_scoreObject->selectedItem()) {
    auto selectedSegment = m_scoreObject->noteSegment(m_scoreObject->selectedItem()->index());
    Ttechnical dataToSet = selectedSegment->technical();
    if (!dataToSet.bowing()) { // no bowing, so look up for any previous note with bowing mark
      for (int i = selectedSegment->index(); i >= 0; --i) {
        auto searchNoteData = m_scoreObject->noteSegment(i)->techicalData();
        if (searchNoteData.bowing()) {
          dataToSet.setBowing(searchNoteData.bowing());
          break;
        }
      }
    }
    technical = dataToSet.data();
  }
  return technical;
}


void TnootkaQML::selectPlayingNote(int id) {
  m_ignoreScore = true;
  m_scoreObject->setSelectedItem(m_scoreObject->note(id));
  auto n = m_scoreObject->selectedNote();
  if (n.isValid())
    n.transpose(GLOB->transposition());
  m_instrument->setNote(n, getTechicalFromScore());
}


int TnootkaQML::scoreNotesCount() const {
  return m_scoreObject->notesCount();
}


QList<Tnote>& TnootkaQML::scoreNoteList() const {
  return m_scoreObject->noteList();
}

