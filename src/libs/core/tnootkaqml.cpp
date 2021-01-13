/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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
#include <QtCore/qtimer.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qbuffer.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qdesktopservices.h>
#include <QtGui/qpalette.h>

#include "Android/tfiledialog.h"

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

  qsrand(QDateTime::currentDateTimeUtc().toTime_t());

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


Tnote TnootkaQML::emptyNote() {
  return Tnote();
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


bool TnootkaQML::isAndroid()  {
#if defined (Q_OS_ANDROID)
  return true;
#else
  return false;
#endif
}


bool TnootkaQML::isWindows() {
#if defined (Q_OS_WIN)
  return true;
#else
  return false;
#endif
}


bool TnootkaQML::isMac() {
#if defined (Q_OS_MACOS)
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
  openFile = TfileDialog::getOpenFileName(GLOB->lastXmlDir(), QStringLiteral("xml|musicxml"));
#else
  openFile = TfileDialog::getOpenFileName(qApp->translate("TmainScoreObject", "Open melody file"), GLOB->lastXmlDir(),
                                          qApp->translate("TmainScoreObject", "MusicXML file") + QLatin1String(" (*.xml *.musicxml)"));
#endif
  if (!openFile.isEmpty())
    GLOB->setLastXmlDir(QFileInfo(openFile).absoluteDir().path());
  return openFile;
}


QString TnootkaQML::getXmlToSave(const QString& fileName) {
  QString saveFile;
  QString filter;
#if defined (Q_OS_ANDROID)
  saveFile = TfileDialog::getSaveFileName(GLOB->lastXmlDir() + QLatin1String("/") + fileName,
                                          QStringLiteral("musicxml|xml"));
#else
  saveFile = TfileDialog::getSaveFileName(qApp->translate("TmainScoreObject", "Save melody as:"), GLOB->lastXmlDir() + QDir::separator() + fileName,
                                          qTR("TmainScoreObject", "MusicXML file") + QLatin1String(" (*.musicxml *.xml)"), &filter);
#endif
  if (!saveFile.isEmpty())
    GLOB->setLastXmlDir(QFileInfo(saveFile).absoluteDir().path());
  return saveFile;
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


qreal TnootkaQML::hue(const QColor& c) const {
  return c.hueF();
}


qreal TnootkaQML::saturation(const QColor& c) const {
  return c.saturationF();
}


qreal TnootkaQML::lightness(const QColor& c) const {
  return c.saturationF();
}


int TnootkaQML::fontSize() {
#if defined (Q_OS_ANDROID)
  // Set Android font according to screen size/density
  return Tmtr::fingerPixels() * 0.3 * GLOB->guiScale();
#else
  // but use system font size on desktops
  return Tmtr::systemFont.pointSize() * GLOB->guiScale();
#endif
}

QString TnootkaQML::fontFamily() {
  return Tmtr::systemFont.family();
}


int TnootkaQML::fingerPixels() { return Tmtr::fingerPixels(); }

int TnootkaQML::shortScreenSide() { return Tmtr::shortScreenSide(); }

int TnootkaQML::longScreenSide() { return Tmtr::longScreenSide(); }


QString TnootkaQML::pixToHtml(const QString& pixName, int height) {
  if (height == 0)
    return QString("<img src=\"%1\">").arg(pixName);

  QPixmap pix;
  if (!pix.load(Tpath::img(qPrintable(pixName))))
    return QString();

  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  pix.scaled(qRound(height * (static_cast<qreal>(pix.width()) / static_cast<qreal>(pix.height()))),
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


void TnootkaQML::setQmlEngine(QQmlEngine* e) {
  m_qmlEngine = e;
  if (GLOB->isFirstRun) // Wizard - actions are not needed yet
    return;

  if (m_scoreAct) {
    delete m_scoreAct;
    delete m_settingsAct;
    delete m_levelAct;
    delete m_chartsAct;
    delete m_melodyAct;
    delete m_examAct;
    delete m_aboutAct;
  }
  m_settingsAct = new Taction(QApplication::translate("TtoolBar", "Settings"), QStringLiteral("systemsettings"), this);
  connect(m_settingsAct, &Taction::triggered, this, &TnootkaQML::settingsActTriggered);
  m_settingsAct->setTip(QApplication::translate("TtoolBar", "Application preferences"), QQuickItem::TopRight);
  m_levelAct = new Taction(QApplication::translate("TtoolBar", "Level"), QStringLiteral("levelCreator"), this);
  connect(m_levelAct, &Taction::triggered, this, &TnootkaQML::levelActTriggered);
  m_levelAct->setTip(QApplication::translate("TtoolBar", "Level creator"), QQuickItem::TopRight);
  m_chartsAct = new Taction(QApplication::translate("TtoolBar", "Analyze"), QStringLiteral("charts"), this);
  connect(m_chartsAct, &Taction::triggered, this, &TnootkaQML::chartsActTriggered);
  m_chartsAct->setTip(tr("Analysis of exam results"), QQuickItem::TopRight);
  m_scoreAct = new Taction(QApplication::translate("TmainScoreObject", "Score", "it could be 'notation', 'staff' or whatever is associated with that 'place to display musical notes' and this the name is quite short and looks well."), QStringLiteral("score"), this);
  m_scoreAct->setBgColor(qApp->palette().highlight().color());
  connect(m_scoreAct, &Taction::triggered, this, &TnootkaQML::scoreActTriggered);
  m_scoreAct->setTip(QApplication::translate("TmainScoreObject", "Manage and navigate the score."), QQuickItem::TopRight);
  m_melodyAct = new Taction(QApplication::translate("TmainScoreObject", "Melody"), QStringLiteral("melody"), this);
  connect(m_melodyAct, &Taction::triggered, this, &TnootkaQML::melodyActTriggered);
  m_melodyAct->setTip(QApplication::translate("TmainScoreObject", "Open, save, generate and play a melody."), QQuickItem::TopRight);
  m_examAct = new Taction(QApplication::translate("TtoolBar", "Lessons"), QStringLiteral("startExam"), this);
  connect(m_examAct, &Taction::triggered, this, &TnootkaQML::examActTriggered);
  m_examAct->setTip(QApplication::translate("TtoolBar", "Start exercises or an exam"), QQuickItem::TopRight);
  m_aboutAct = new Taction(this);
  connect(m_aboutAct, &Taction::triggered, this, &TnootkaQML::aboutActTriggered);
  m_messageColor = qApp->palette().highlight().color();
}


/**
 * Opening files from command line argument starts here, but it is a bit clumsy:
 * - for Music XML is fine, we have @p TscoreObject here so just call @p openMusicXml()
 * - but for exam and level files only @p wantOpenFile() signal is emitted
 *   with 700ms delay to give main window time for initialize
 * - then @p MainWindow.qml handles it and creates @p DialogLoader.qml
 * - 'dialog loader' invokes @p TdialogLoaderObject::openFile() of its object
 * - and there the file is distinguished (exam or level) and appropriate signal is emitted
 * - again, 'dialog loader' handles those signals and creates 'exam executor' or 'level creator' apparently
 */
void TnootkaQML::openFile(const QString& runArg) {
  if (GLOB->isExam()) {
    qDebug() << "--- Exam or exercise is running. File cannot be opened! ---";
    return;
  }
  if (QFile::exists(runArg)) {
    QFile file(runArg);
    auto ext = QFileInfo(file).suffix();
    if (ext == QLatin1String("xml") || ext == QLatin1String("musicxml")) {
        auto fullPath = QDir(file.fileName()).absolutePath();
        m_scoreObject->openMusicXml(fullPath);
    } else {
        QTimer::singleShot(700, this, [=]{ emit GLOB->wantOpenFile(runArg); });
    }
  }
}


void TnootkaQML::setMessageColor(const QColor& mc) {
  if (m_messageColor != mc) {
    m_messageColor = mc;
    emit messageColorChanged();
  }
}


void TnootkaQML::setStatusTip(const QString& statusText, int tipPos) {
  if ((GLOB->showHints() && (!m_messageTimer || (m_messageTimer && !m_messageTimer->isActive()))))
    emit statusTip(statusText, tipPos);
}


void TnootkaQML::showTimeMessage(const QString& message, int time, int pos) {
  if (!m_messageTimer) {
    m_messageTimer = new QTimer(this);
    m_messageTimer->setSingleShot(true);
    connect(m_messageTimer, &QTimer::timeout, this, [=]{
      emit statusTip(QString(), m_messagePos);
      QTimer::singleShot(300, this, [=] { setMessageColor(qApp->palette().highlight().color()); } );// restore default status background color
    });
  }
  m_messagePos = pos;
  if (m_messageTimer->isActive())
    m_messageTimer->stop();

  emit statusTip(message, pos);
  m_messageTimer->start(time);
}


bool TnootkaQML::messageTimerActive() const {
  return m_messageTimer ? m_messageTimer->isActive() : false;
}



QString TnootkaQML::qaTypeText(int qaType) {
  switch (qaType) {
    case 0: return QApplication::translate("Texam", "as note on the staff");
    case 1: return QApplication::translate("Texam", "as note name");
    case 2: return QApplication::translate("Texam", "on instrument");
    case 3: return QApplication::translate("Texam", "as played sound");
    default: return QString();
  }
}


QString TnootkaQML::note7translated() const {
  return QApplication::translate("Notation", "b", "Give here a name of 7-th note preferred in your country. But only 'b' or 'h' not 'si' or something worst...");
}


QString TnootkaQML::keyNameTranslated() const {
  return QApplication::translate("Notation", "letters", "DO NOT TRANSLATE IT DIRECTLY. Put here 'letters' or 'solfege' This is country preferred style of naming key signatures. 'letters' means C-major/a-minor names ('major' & 'minor' also are translated by you), 'solfege' means Do-major/La-minor names");
}


//#################################################################################################
//###################     CONNECTIONS  NODE            ############################################
//#################################################################################################
void TnootkaQML::noteStarted(const Tnote& n) {
  Tnote note = n;
  if (m_scoreObject->keySignature() < 0 || (m_scoreObject->keySignature() == 0 && GLOB->GpreferFlats))
    note = note.showWithFlat();
  m_ignoreScore = true;
  if (m_scoreObject->singleNote()) {
      if (!note.isRest()) {
        note.setRhythm(Trhythm::NoRhythm);
        m_scoreObject->setNote(0, note);
      }
  } else
      m_scoreObject->addNote(note, true);
}


void TnootkaQML::noteFinished(const Tnote& n) {
  Tnote note = n;
  if (m_instrument)
    m_instrument->setNote(note);
  if (m_scoreObject->keySignature() < 0 || (m_scoreObject->keySignature() == 0 && GLOB->GpreferFlats))
    note = note.showWithFlat();
  m_ignoreScore = true;
  if (m_scoreObject->singleNote())
    note.setRhythm(Trhythm::NoRhythm);
  if (m_scoreObject->selectedItem() && (!m_scoreObject->singleNote() || (m_scoreObject->singleNote() && !note.isRest())))
    m_scoreObject->setNote(m_scoreObject->selectedItem(), note);
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
    if (m_scoreObject && !m_nodeConnected && !GLOB->isExam())
      connectInstrument();
  }
}


void TnootkaQML::connectInstrument() {
  m_nodeConnected = true;
  if (m_instrument)
    connect(m_instrument, &TcommonInstrument::noteChanged, this, &TnootkaQML::instrumentChangesNoteSlot);
}


void TnootkaQML::instrumentChangesNoteSlot() {
  m_ignoreScore = true;
  Tnote noteToPlay = m_instrument->note();
  noteToPlay.transpose(GLOB->transposition());
  emit playNote(noteToPlay);
  Tnote instrNote = m_instrument->note();
  if (m_scoreObject->keySignature() < 0 || (m_scoreObject->keySignature() == 0 && GLOB->GpreferFlats))
    instrNote = instrNote.showWithFlat();

  if (m_scoreObject->singleNote()) {
      m_scoreObject->setNote(0, instrNote);
      m_scoreObject->setTechnical(0, m_instrument->technical());
  } else {
      if (m_scoreObject->selectedItem()) {
          auto r = m_scoreObject->selectedItem()->note()->rtm;
          r.setRest(false);
          instrNote.setRhythm(r);
          m_scoreObject->setNote(m_scoreObject->selectedItem(), instrNote);
      } else {
          instrNote.setRhythm(m_scoreObject->workRhythm());
          m_scoreObject->addNote(instrNote, true);
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
  auto scoreNote = m_scoreObject->selectedNote();
  if (m_instrument)
    m_instrument->setNote(scoreNote, getTechicalFromScore());
  if (scoreNote.isValid())
    scoreNote.transpose(GLOB->transposition());
  emit playNote(scoreNote);
}


int TnootkaQML::selectedNoteId() const {
  return m_scoreObject->selectedItem() ? m_scoreObject->selectedItem()->index() : -1;
}


int TnootkaQML::getTechicalFromScore() {
  quint32 technical = NO_TECHNICALS; // empty by default
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
  if (m_instrument)
    m_instrument->setNote(m_scoreObject->selectedNote(), getTechicalFromScore());
}


int TnootkaQML::scoreNotesCount() const {
  return m_scoreObject->notesCount();
}


QList<Tnote>& TnootkaQML::scoreNoteList() const {
  return m_scoreObject->noteList();
}

