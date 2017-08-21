/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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

#include "tglobals.h"
#include "nootkaconfig.h"
#include <music/tkeysignature.h>
#include <music/ttuneobject.h>
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
#include <music/tnamestylefilter.h>
#include "tpath.h"
#include "tlayoutparams.h"
#include "tinitcorelib.h"
#include "tcolor.h"
#if defined (Q_OS_ANDROID)
  #include <Android/tandroid.h>
#endif
#include <QtCore/qdir.h>
#include <QtCore/qsettings.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtGui/qscreen.h>
#include <QtCore/qdebug.h>


/*static*/
QString& Tglobals::path = Tpath::main;
Tglobals* Tglobals::m_instance = nullptr;

QString Tglobals::getInstPath(QString appInstPath) {
    QString p;
    QDir d = QDir(appInstPath);
#if defined (Q_OS_WIN)
    p = d.path() + QLatin1String("/");         //  Windows
  #elif defined (Q_OS_ANDROID)
    p = ":/";
  #elif defined (Q_OS_LINUX)
      d.cdUp();
      p = d.path() + QLatin1String("/share/nootka/");   // Linux
  #elif defined (Q_OS_MAC)
      d.cdUp();
      p = d.path() + QLatin1String("/Resources/");     // MacOs
#endif

  return p;
}

/*end static*/


Tglobals::Tglobals(QObject* parent) :
  QObject(parent),
  m_tune(nullptr)
{
  version = NOOTKA_VERSION;

  qRegisterMetaTypeStreamOperators<Ttune>("Ttune");
  qRegisterMetaType<Tnote>("Tnote");

  QCoreApplication::setOrganizationName(QStringLiteral("Nootka"));
  QCoreApplication::setOrganizationDomain(QStringLiteral("nootka.sf.net"));
  if (qApp->applicationName() != QLatin1String("Nootini"))
    QCoreApplication::setApplicationName(QStringLiteral("Nootka2"));

  E = new TexamParams();
  A = new TaudioParams();
  S = new TscoreParams();
  L = new TlayoutParams();
  m_tuneObject = new TtuneObject(this);

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC) // I hate mess in Win registry
  config = new QSettings(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("Nootka"), qApp->applicationName());
#else
  config = new QSettings();
#endif
  loadSettings(config);

  if (!m_instance) {
      Tcore::setGlobals(this); // TODO remove it, use GLOB macro instead
      m_instance = this;
  } else {
      qDebug() << "Tglobals instance has already existed. Application is terminating!";
      exit(109);
  }
}


Tglobals::~Tglobals() {
  storeSettings(config);
  delete E;
  delete A;
  delete S;
  delete L;
  delete m_tune;
  delete config;
  Tcore::reset();
  m_instance = nullptr;
}


//##########################################################################################
//#######################         PUBLIC         ###########################################
//##########################################################################################

void Tglobals::setUseAnimations(bool use) { m_useAnimations = use; emit useAnimationsChanged(); }

bool Tglobals::showEnharmNotes() const { return S->showEnharmNotes; }
void Tglobals::setShowEnharmNotes(bool showEnharm) { S->showEnharmNotes = showEnharm; }

QColor Tglobals::getEnharmNoteColor() const { return S->enharmNotesColor; }
void Tglobals::setEnharmNoteColor(const QColor& c) { S->enharmNotesColor = c; }

QColor Tglobals::getNoteCursorColor() const { return S->pointerColor; }
void Tglobals::setNoteCursorColor(const QColor& c) { S->pointerColor = c; emit noteCursorColorChanged(); }

bool Tglobals::isSingleNote() const { return S->isSingleNoteMode; }
void Tglobals::setSingleNote(bool sn) { S->isSingleNoteMode = sn; }

bool Tglobals::enableDoubleAccids() const { return S->doubleAccidentalsEnabled; }
void Tglobals::setEnableDoubleAccids(bool dblAcc) { S->doubleAccidentalsEnabled = dblAcc; emit enableDoubleAccidsChanged(); }

bool Tglobals::keySignatureEnabled() const { return S->keySignatureEnabled; }
void Tglobals::setKeySignatureEnabled(bool enKey) { S->keySignatureEnabled = enKey; emit enableKeySignatureChanged(); }

int Tglobals::clefType() const { return static_cast<int>(S->clef); }
void Tglobals::setClefType(int clType) { S->clef = static_cast<Tclef::EclefType>(clType); emit clefTypeChanged(); }

QString Tglobals::majorKeyNameSufix() const { return S->majKeyNameSufix; }
void Tglobals::setMajorKeyNameSufix(const QString& mkns) { S->majKeyNameSufix = mkns; }

QString Tglobals::minorKeyNameSufix() const { return S->minKeyNameSufix; }
void Tglobals::setMinorKeyNameSufix(const QString& mkns) { S->minKeyNameSufix = mkns; }

int Tglobals::keyNameStyle() const { return static_cast<int>(S->nameStyleInKeySign); }
void Tglobals::setKeyNameStyle(int keyStyle) { S->nameStyleInKeySign = static_cast<Tnote::EnameStyle>(keyStyle); }

bool Tglobals::showKeyName() const { return S->showKeySignName; }
void Tglobals::setShowKeyName(bool showKey) { S->showKeySignName = showKey; emit showKeyNameChanged(); }


void Tglobals::updateKeySignatureNames() {
  TkeySignature::setNameStyle(S->nameStyleInKeySign, S->majKeyNameSufix, S->minKeyNameSufix);
  emit keyNameChanged();
}

bool Tglobals::rhythmsEnabled() const { return S->rhythmsEnabled; }
void Tglobals::setRhythmsEnabled(bool enR) {
  if (S->rhythmsEnabled != enR) {
    S->rhythmsEnabled = enR;
    emit rhythmsEnabledChanged();
  }
}

/* ------------------ Note name switches ------------------ */
bool Tglobals::seventhIsB() const { return S->seventhIs_B; }

void Tglobals::setSeventhIsB(bool isB) {
  if (isB != S->seventhIs_B) {
    S->seventhIs_B = isB;
    emit seventhIsBChanged();
  }
}

int Tglobals::noteNameStyle() const { return static_cast<int>(S->nameStyleInNoteName); }

void Tglobals::setNoteNameStyle(int nameStyle) {
  Tnote::EnameStyle newNameStyle = static_cast<Tnote::EnameStyle>(nameStyle);
  if (newNameStyle != S->nameStyleInNoteName) {
    S->nameStyleInNoteName = static_cast<Tnote::EnameStyle>(nameStyle);
    Tnote::defaultStyle = S->nameStyleInNoteName;
    emit noteNameStyleChanged();
  }
}

bool Tglobals::namesOnScore() const { return S->namesOnScore; }

void Tglobals::setNamesOnScore(bool showNames) {
  if (showNames != S->namesOnScore) {
    S->namesOnScore = showNames;
    emit namesOnScoreChanged();
  }
}

QColor Tglobals::nameColor() const { return S->nameColor; }

void Tglobals::setNameColor(const QColor& nameC) {
  if (nameC != S->nameColor) {
    S->nameColor = nameC;
    emit nameColorChanged();
  }
}

/* ------------------ Instrument switches ------------------ */
void Tglobals::setInstrument(Tinstrument::Etype t) {
  if (t != m_instrument.type()) {
    m_instrument.setType(t);
    emit instrumentChanged();
  }
}


int Tglobals::transposition() const { return A->transposition; }
void Tglobals::setTransposition(int t) {
  if (t != A->transposition) {
    A->transposition = t;
    emit transpositionChanged();
  }
}

QString Tglobals::markedFrets() const {
  QString fretText;
  for (int i = 0; i < GmarkedFrets.size(); ++i) {
    fretText.append(GmarkedFrets.at(i).toString());
    if (i < GmarkedFrets.size() - 1)
      fretText.append(QStringLiteral(","));
  }
  return fretText;
}


void Tglobals::setMarkedFrets(const QString& frets) {
  GmarkedFrets.clear();
  QString ex = QStringLiteral("!");
  QStringList fr = frets.split(QStringLiteral(","));
  for (int i = 0; i < fr.size(); ++i) {
    QString exMark;
    if (fr[i].contains(ex)) {
      exMark = ex;
      fr[i].replace(ex, QString());
    }
    bool ok;
    int frNr = fr[i].toInt(&ok);
    if (ok && frNr > 0 && frNr <= GfretsNumber)
      GmarkedFrets << fr[i] + exMark;
  }
}


/* ------------------ Sound switches ------------------ */
int Tglobals::audioInstrument() const { return A->audioInstrNr; }
void Tglobals::setAudioInstrument(int ai) { A->audioInstrNr = ai; }


void Tglobals::setGuitarParams(int fretNr, int stringNr) {
  bool doEmit = false;
  if (fretNr != GfretsNumber) {
    GfretsNumber = fretNr;
    doEmit = true;
  }
  if (stringNr != m_tune->stringNr()) {
    // TODO
  }
  if (doEmit)
    emit guitarParamsChanged();
}



int Tglobals::stringNumber() { return m_tune->stringNr(); }


void Tglobals::setFingerColor(const QColor& fc) { GfingerColor = fc; emit fingerColorChanged(); }
void Tglobals::setSelectedColor(const QColor& sc) { GselectedColor = sc; emit selectedColorChanged(); }


void Tglobals::loadSettings(QSettings* cfg) {
  cfg->beginGroup(QLatin1String("General"));
    m_geometry = cfg->value(QStringLiteral("geometry"), QRect()).toRect();
    if (m_geometry.width() < 720 || m_geometry.height() < 480) {
      m_geometry.setWidth(qMax(qRound(qApp->primaryScreen()->size().width() * 0.75), 720));
      m_geometry.setHeight(qMax(qRound(qApp->primaryScreen()->size().height() * 0.75), 480));
      m_geometry.setX((qApp->primaryScreen()->size().width() - m_geometry.width()) / 2);
      m_geometry.setY((qApp->primaryScreen()->size().height() - m_geometry.height()) / 2);
    }
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("common"));
      isFirstRun = cfg->value(QStringLiteral("isFirstRun"), true).toBool();
      m_useAnimations = cfg->value(QStringLiteral("useAnimations"), true).toBool();
      lang = cfg->value(QStringLiteral("language"), QString()).toString();
  cfg->endGroup();

//score widget settings
  cfg->beginGroup(QLatin1String("score"));
      S->keySignatureEnabled = cfg->value(QStringLiteral("keySignature"), false).toBool();
      S->showKeySignName = cfg->value(QStringLiteral("keyName"), true).toBool(); //true;
      S->nameStyleInKeySign = Tnote::EnameStyle(cfg->value(QStringLiteral("nameStyleInKey"),
                                                          (int)Tnote::e_english_Bb).toInt());
      S->majKeyNameSufix = cfg->value(QStringLiteral("majorKeysSufix"), QString()).toString();
      S->minKeyNameSufix = cfg->value(QStringLiteral("minorKeysSufix"), QString()).toString();
      if (cfg->contains("pointerColor"))
          S->pointerColor = cfg->value(QStringLiteral("pointerColor")).value<QColor>();
      else
          S->pointerColor = Tcolor::invert(qApp->palette().highlight().color());
      S->clef = Tclef::EclefType(cfg->value(QStringLiteral("clef"), (int)Tclef::Treble_G_8down).toInt());
      // Rhythms has to be enabled when no clef (percussion)
      S->rhythmsEnabled = cfg->value(QStringLiteral("rhythmsEnabled"), true).toBool() || S->clef == Tclef::NoClef;
      S->isSingleNoteMode = cfg->value(QStringLiteral("singleNoteMode"), false).toBool();
      S->tempo = cfg->value(QStringLiteral("tempo"), 120).toInt();
      S->scoreScale = cfg->value(QStringLiteral("scoreScale"), 1.0).toReal();
  cfg->endGroup();


//common for score widget and note name
  cfg->beginGroup(QLatin1String("common"));
      S->doubleAccidentalsEnabled = cfg->value(QStringLiteral("doubleAccidentals"), false).toBool();
      S->showEnharmNotes = cfg->value(QStringLiteral("showEnaharmonicNotes"), false).toBool();
      if (!S->isSingleNoteMode) // enharmonically equal notes can be enabled only in single note mode
          S->showEnharmNotes = false;
      S->enharmNotesColor = cfg->value(QStringLiteral("enharmonicNotesColor"), QColor(0, 162, 162)).value<QColor>(); // turquoise
      S->seventhIs_B = cfg->value(QStringLiteral("is7thNote_B"), true).toBool(); //true;
  cfg->endGroup();

//note name settings
  cfg->beginGroup(QLatin1String("noteName"));
      S->nameStyleInNoteName = Tnote::EnameStyle(cfg->value(QStringLiteral("nameStyle"), (int)Tnote::e_english_Bb).toInt());
      S->solfegeStyle = Tnote::EnameStyle(cfg->value(QStringLiteral("solfegeStyle"), (int)getSolfegeStyle()).toInt());
      S->octaveInNoteNameFormat = cfg->value(QStringLiteral("octaveInName"), true).toBool();
      S->namesOnScore = cfg->value(QStringLiteral("namesOnScore"), true).toBool();
      S->nameColor = cfg->value(QStringLiteral("namesColor"), QColor(0, 225, 225)).value<QColor>();
  cfg->endGroup();
// Fix name style depending on 7th note is was set wrongly in configuration (naughty user)
  if (S->seventhIs_B) {
      if (S->nameStyleInNoteName == Tnote::e_norsk_Hb)
        S->nameStyleInNoteName = Tnote::e_english_Bb;
      else if (S->nameStyleInNoteName == Tnote::e_deutsch_His)
        S->nameStyleInNoteName = Tnote::e_nederl_Bis;
  } else {
      if (S->nameStyleInNoteName == Tnote::e_english_Bb)
        S->nameStyleInNoteName = Tnote::e_norsk_Hb;
      else if (S->nameStyleInNoteName == Tnote::e_nederl_Bis)
        S->nameStyleInNoteName = Tnote::e_deutsch_His;
  }
// Initialize name filter
  TnameStyleFilter::setStyleFilter(&S->seventhIs_B, &S->solfegeStyle);
  Tnote::defaultStyle = S->nameStyleInNoteName;

// guitar settings
  Ttune::prepareDefinedTunes();
  cfg->beginGroup(QLatin1String("guitar"));
      m_instrument.setType(static_cast<Tinstrument::Etype>(cfg->value(QStringLiteral("instrument"), static_cast<int>(Tinstrument::ClassicalGuitar)).toInt()));
      GfretsNumber = cfg->value(QStringLiteral("fretNumber"), 19).toInt();
      GisRightHanded = cfg->value(QStringLiteral("rightHanded"), true).toBool(); //true;
      GshowOtherPos = cfg->value(QStringLiteral("showOtherPos"), false).toBool();
      if (cfg->contains("fingerColor"))
          GfingerColor = cfg->value(QStringLiteral("fingerColor")).value<QColor>();
      else
          GfingerColor = QColor(255, 0, 127, 200); // nice pink
      if (cfg->contains("selectedColor"))
          GselectedColor = cfg->value(QStringLiteral("selectedColor")).value<QColor>();
      else
          GselectedColor = QColor(51, 153, 255); // nice blue as default
      QVariant tun = cfg->value(QStringLiteral("tune"));
      if (tun.isValid()) {
          Ttune tmpTune = tun.value<Ttune>();
          setTune(tmpTune);
      } else
          setTune(Ttune::stdTune);
      GpreferFlats = cfg->value(QStringLiteral("flatsPrefered"), false).toBool(); //false;
      QList<QVariant> fretsList;
      fretsList << 5 << 7 << 9 << "12!" << 15 << 17;
      GmarkedFrets = cfg->value(QStringLiteral("dotsOnFrets"), fretsList).toList();
  cfg->endGroup();

// Exam settings
  cfg->beginGroup(QLatin1String("exam"));
      if (cfg->contains(QLatin1String("questionColor")))
          EquestionColor = cfg->value(QStringLiteral("questionColor")).value<QColor>();
      else {
          EquestionColor = QColor(255, 0, 0); // red
          EquestionColor.setAlpha(40);
      }
      if (cfg->contains(QLatin1String("answerColor")))
          EanswerColor = cfg->value(QStringLiteral("answerColor")).value<QColor>();
      else {
          EanswerColor = QColor(0, 255, 0); // green
          EanswerColor.setAlpha(40);
      }
      if (cfg->contains(QLatin1String("notBadColor")))
          EnotBadColor = cfg->value(QStringLiteral("notBadColor")).value<QColor>();
      else {
          EnotBadColor = QColor(255, 128, 0); // #FF8000
          EnotBadColor.setAlpha(40);
      }
      E->autoNextQuest = cfg->value(QStringLiteral("autoNextQuest"), false).toBool();
      E->repeatIncorrect = cfg->value(QStringLiteral("repeatIncorrect"), true).toBool();
      E->expertsAnswerEnable = cfg->value(QStringLiteral("expertsAnswerEnable"), false).toBool();
      E->studentName = cfg->value(QStringLiteral("studentName"), QString()).toString();
#if defined (Q_OS_ANDROID)
      E->examsDir = cfg->value(QStringLiteral("examsDir"), Tandroid::getExternalPath()).toString();
      if (!QFileInfo::exists(E->examsDir)) // reset if doesn't exist
        E->examsDir = Tandroid::getExternalPath();
      E->levelsDir = cfg->value(QStringLiteral("levelsDir"), Tandroid::getExternalPath()).toString();
      if (!QFileInfo::exists(E->levelsDir))
        E->levelsDir = Tandroid::getExternalPath();
#else
      E->examsDir = cfg->value(QStringLiteral("examsDir"), QDir::homePath()).toString();
      if (!QFileInfo::exists(E->examsDir)) // reset if doesn't exist
        E->examsDir = QDir::homePath();
      E->levelsDir = cfg->value(QStringLiteral("levelsDir"), QDir::homePath()).toString();
      if (!QFileInfo::exists(E->levelsDir)) // reset if doesn't exist
        E->levelsDir = QDir::homePath();
#endif
      E->closeWithoutConfirm = cfg->value(QStringLiteral("closeWithoutConfirm"), false).toBool();
      E->showCorrected = cfg->value(QStringLiteral("showCorrected"), true).toBool();
      E->mistakePreview = cfg->value(QStringLiteral("mistakePreview"), 3000).toInt();
      E->correctPreview = cfg->value(QStringLiteral("correctPreview"), 3000).toInt();
      E->questionDelay = cfg->value(QStringLiteral("questionDelay"), 150).toInt();
      E->suggestExam = cfg->value(QStringLiteral("suggestExam"), true).toBool();
      E->afterMistake = (TexamParams::EafterMistake)cfg->value(QStringLiteral("afterMistake"), (int)TexamParams::e_continue).toInt();
      E->showNameOfAnswered = cfg->value(QStringLiteral("showNameOfAnswered"), true).toBool();
      E->showWrongPlayed = cfg->value(QStringLiteral("showWrongPlayed"), false).toBool();
      E->showHelpOnStart = cfg->value(QStringLiteral("showHelpOnStart"), true).toBool();
      E->askAboutExpert = cfg->value(QStringLiteral("askAboutExpert"), true).toBool();
      E->showVeryBeginHelp = cfg->value(QStringLiteral("showVeryBeginHelp"), false).toBool();
  cfg->endGroup();

// Sound settings
  cfg->beginGroup(QLatin1String("sound"));
    A->JACKorASIO = cfg->value(QStringLiteral("JACKorASIO"), false).toBool();
    A->OUTenabled = cfg->value(QStringLiteral("outSoundEnabled"), true).toBool();
    A->OUTdevName = cfg->value(QStringLiteral("outDeviceName"), QString()).toString();
    A->midiEnabled = cfg->value(QStringLiteral("midiEnabled"), false).toBool();
    A->midiPortName = cfg->value(QStringLiteral("midiPortName"), QString()).toString();
    A->midiInstrNr = (unsigned char)cfg->value(QStringLiteral("midiInstrumentNr"), 0).toInt();
    A->audioInstrNr = qBound(1, cfg->value(QStringLiteral("audioInstrumentNr"), 1).toInt(), INSTR_COUNT);
    A->INenabled = cfg->value(QStringLiteral("inSoundEnabled"), true).toBool();
    A->INdevName = cfg->value(QStringLiteral("inDeviceName"), QString()).toString();
    A->detectMethod = qBound(0, cfg->value(QStringLiteral("detectionMethod"), 2).toInt(), 2); // MPM modified cepstrum
#if defined (Q_OS_ANDROID) // Input sound is loud on mobile
    A->minimalVol = cfg->value(QStringLiteral("minimalVolume"), 0.6).toFloat();
#else
    A->minimalVol = cfg->value(QStringLiteral("minimalVolume"), 0.4).toFloat();
    A->dumpPath = cfg->value(QLatin1String("dumpPath"), QString()).toString();
#endif
    A->minDuration = cfg->value(QStringLiteral("minimalDuration"), 0.15).toFloat(); // 150 ms
    A->a440diff = cfg->value(QStringLiteral("a440Offset"), 0).toFloat();
    A->intonation = (quint8)qBound(0, cfg->value(QStringLiteral("intonation"), 3).toInt(), 5);
    A->forwardInput = cfg->value(QStringLiteral("forwardInput"), false).toBool();
    A->playDetected = false; //cfg->value(QStringLiteral("playDetected"), false).toBool();
    A->equalLoudness = cfg->value(QStringLiteral("equalLoudness"), true).toBool();
    A->minSplitVol = cfg->value(QStringLiteral("minVolumeToSplit"), 10.0).toReal();
    A->skipStillerVal = cfg->value(QStringLiteral("skipStillerThan"), 80.0).toReal();
    A->transposition = cfg->value(QStringLiteral("transposition"), 0).toInt();
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("layout"));
    L->guitarEnabled = cfg->value(QStringLiteral("guitarEnabled"), true).toBool();
#if defined (Q_OS_ANDROID)
    L->soundViewEnabled = cfg->value(QStringLiteral("soundViewEnabled"), false).toBool();
  // override some options not supported under mobile systems
  L->toolBarAutoHide = true;
  L->iconTextOnToolBar = Qt::ToolButtonTextBesideIcon;
  L->hintsBarEnabled = false;
  GisRightHanded = true;
#else
    L->soundViewEnabled = cfg->value(QStringLiteral("soundViewEnabled"), true).toBool();
    L->toolBarAutoHide = cfg->value(QStringLiteral("toolBarAutoHide"), false).toBool();
    L->iconTextOnToolBar = Qt::ToolButtonStyle(cfg->value(QStringLiteral("iconTextOnToolBar"), 3).toInt());
    L->hintsBarEnabled = cfg->value(QStringLiteral("hintsBarEnabled"), true).toBool();
#endif
  cfg->endGroup();

//   cfg->beginGroup(QLatin1String("touch"));
//     TtouchParams::i()->scoreWasTouched = cfg->value(QStringLiteral("scoreWasTouched"), false).toBool();
//     TtouchParams::i()->clefWasTouched = cfg->value(QStringLiteral("clefWasTouched"), false).toBool();
//     TtouchParams::i()->guitarWasTouched = cfg->value(QStringLiteral("guitarWasTouched"), false).toBool();
//     TtouchParams::i()->initialAnimAccepted = cfg->value(QStringLiteral("initialAnimAccepted"), false).toBool();
//   cfg->endGroup();
}


void Tglobals::setTune(Ttune& t) {
  delete m_tune;
  m_tune = new Ttune(t.name, t[1], t[2], t[3], t[4], t[5], t[6]);
  m_tuneObject->setTune(m_tune);
  // creating array with guitar strings in order of their height
  char openStr[6];
  for (int i = 0; i < 6; i++) {
    m_order[i] = i;
    if (m_tune->str(i + 1).note != 0)
      openStr[i] = m_tune->str(i + 1).chromatic();
    else // empty note - not such string
      openStr[i] = -120; // make it lowest
  }
  int i = 4;
  while (i > -1) {
    for (int j = i; j < 5 && openStr[m_order[j]] < openStr[m_order[j + 1]]; j++) {
      char tmp = m_order[j];
      m_order[j] = m_order[j + 1];
      m_order[j + 1] = tmp;
    }
    i--;
  }
  emit tuningChanged();
}


Tnote Tglobals::hiString() {
  return m_tune->str(m_order[0] + 1);
}


Tnote Tglobals::loString() {
    return m_tune->str(m_order[m_tune->stringNr() - 1] + 1);
}


Tnote::EnameStyle Tglobals::getSolfegeStyle() {
    Tnote::EnameStyle solStyle = Tnote::e_italiano_Si;
    QString ll = lang;
    if (ll.isEmpty()) {
      QLocale loc; // default locale (QLocale::setDefault()) grabs local LANG variable in contrary to QLocale::system() which not
      ll = loc.name();
    }
    if (ll.contains(QLatin1String("ru")))
      solStyle = Tnote::e_russian_Ci;
    return solStyle;
}



void Tglobals::storeSettings(QSettings* cfg) {
  cfg->beginGroup(QStringLiteral("General"));
    cfg->setValue(QStringLiteral("geometry"), m_geometry);
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("common"));
      cfg->setValue(QStringLiteral("isFirstRun"), isFirstRun);
      cfg->setValue(QStringLiteral("useAnimations"), m_useAnimations);
      cfg->setValue(QStringLiteral("doubleAccidentals"), S->doubleAccidentalsEnabled);
      cfg->setValue(QStringLiteral("showEnaharmonicNotes"), S->showEnharmNotes);
      cfg->setValue(QStringLiteral("enharmonicNotesColor"), S->enharmNotesColor);
      cfg->setValue(QStringLiteral("is7thNote_B"), S->seventhIs_B);
      cfg->setValue(QStringLiteral("language"), lang);
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("score"));
      cfg->setValue(QStringLiteral("keySignature"), S->keySignatureEnabled);
      cfg->setValue(QStringLiteral("keyName"), S->showKeySignName);
      cfg->setValue(QStringLiteral("nameStyleInKey"), (int)S->nameStyleInKeySign);
  QString majS, minS;
  if (S->majKeyNameSufix != TkeySignature::majorSufixTxt())
    majS = S->majKeyNameSufix;
  else
    majS.clear(); // default suffixes are reset to be translatable in next run
      cfg->setValue(QStringLiteral("majorKeysSufix"), majS);
  if (S->minKeyNameSufix != TkeySignature::minorSufixTxt())
    minS = S->minKeyNameSufix;
  else
    minS.clear();
      cfg->setValue(QStringLiteral("minorKeysSufix"), minS);
      cfg->setValue(QStringLiteral("pointerColor"), S->pointerColor);
      cfg->setValue(QStringLiteral("clef"), (int)S->clef);
      cfg->setValue(QStringLiteral("rhythmsEnabled"), S->rhythmsEnabled);
      cfg->setValue(QStringLiteral("singleNoteMode"), S->isSingleNoteMode);
      cfg->setValue(QStringLiteral("tempo"), S->tempo);
      cfg->setValue(QStringLiteral("scoreScale"), S->scoreScale);
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("noteName"));
      cfg->setValue(QStringLiteral("nameStyle"), (int)S->nameStyleInNoteName);
      cfg->setValue(QStringLiteral("octaveInName"), S->octaveInNoteNameFormat);
      cfg->setValue(QStringLiteral("solfegeStyle"), S->solfegeStyle);
      cfg->setValue(QStringLiteral("namesOnScore"), S->namesOnScore );
      cfg->setValue(QStringLiteral("namesColor"), S->nameColor);
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("guitar"));
      cfg->setValue(QStringLiteral("instrument"), static_cast<int>(m_instrument.type()));
      cfg->setValue(QStringLiteral("fretNumber"), (int)GfretsNumber);
      cfg->setValue(QStringLiteral("rightHanded"), GisRightHanded);
      cfg->setValue(QStringLiteral("showOtherPos"), GshowOtherPos);
      cfg->setValue(QStringLiteral("fingerColor"), GfingerColor);
      cfg->setValue(QStringLiteral("selectedColor"), GselectedColor);
      cfg->setValue(QStringLiteral("tune"), qVariantFromValue(*Gtune()));
      cfg->setValue(QStringLiteral("flatsPrefered"), GpreferFlats);
      cfg->setValue(QStringLiteral("dotsOnFrets"), GmarkedFrets);
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("exam"));
      cfg->setValue(QStringLiteral("questionColor"), EquestionColor);
      cfg->setValue(QStringLiteral("answerColor"), EanswerColor);
      cfg->setValue(QStringLiteral("notBadColor"), EnotBadColor);
      cfg->setValue(QStringLiteral("autoNextQuest"), E->autoNextQuest);
      cfg->setValue(QStringLiteral("repeatIncorrect"), E->repeatIncorrect);
      cfg->setValue(QStringLiteral("showCorrected"), E->showCorrected);
      cfg->setValue(QStringLiteral("expertsAnswerEnable"), E->expertsAnswerEnable);
      cfg->setValue(QStringLiteral("studentName"), E->studentName);
      cfg->setValue(QStringLiteral("examsDir"), E->examsDir);
      cfg->setValue(QStringLiteral("levelsDir"), E->levelsDir);
      cfg->setValue(QStringLiteral("closeWithoutConfirm"), E->closeWithoutConfirm);
      cfg->setValue(QStringLiteral("mistakePreview"), E->mistakePreview);
      cfg->setValue(QStringLiteral("correctPreview"), E->correctPreview);
      cfg->setValue(QStringLiteral("questionDelay"), E->questionDelay);
      cfg->setValue(QStringLiteral("suggestExam"), E->suggestExam);
      cfg->setValue(QStringLiteral("afterMistake"), (int)E->afterMistake);
      cfg->setValue(QStringLiteral("showNameOfAnswered"), E->showNameOfAnswered);
      cfg->setValue(QStringLiteral("showWrongPlayed"), E->showWrongPlayed);
      cfg->setValue(QStringLiteral("askAboutExpert"), E->askAboutExpert);
      cfg->setValue(QStringLiteral("showHelpOnStart"), E->showHelpOnStart);
      cfg->setValue(QStringLiteral("showVeryBeginHelp"), E->showVeryBeginHelp);
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("sound"));
      cfg->setValue(QStringLiteral("JACKorASIO"), A->JACKorASIO);
      cfg->setValue(QStringLiteral("outSoundEnabled"), A->OUTenabled);
      cfg->setValue(QStringLiteral("outDeviceName"), A->OUTdevName);
      cfg->setValue(QStringLiteral("midiEnabled"), A->midiEnabled);
      cfg->setValue(QStringLiteral("midiPortName"), A->midiPortName);
      cfg->setValue(QStringLiteral("midiInstrumentNr"), (int)A->midiInstrNr);
      cfg->setValue(QStringLiteral("audioInstrumentNr"), (int)A->audioInstrNr);
      cfg->setValue(QStringLiteral("inSoundEnabled"), A->INenabled);
      cfg->setValue(QStringLiteral("inDeviceName"), A->INdevName);
      cfg->setValue(QStringLiteral("detectionMethod"), A->detectMethod);
      cfg->setValue(QStringLiteral("minimalVolume"), A->minimalVol);
      cfg->setValue(QStringLiteral("minimalDuration"), A->minDuration);
      cfg->setValue(QStringLiteral("a440Offset"), A->a440diff);
      cfg->setValue(QStringLiteral("intonation"), A->intonation);
      cfg->setValue(QStringLiteral("forwardInput"), A->forwardInput);
//       cfg->setValue(QStringLiteral("playDetected"), A->playDetected);
      cfg->setValue(QStringLiteral("equalLoudness"), A->equalLoudness);
      cfg->setValue(QStringLiteral("minVolumeToSplit"), A->minSplitVol);
      cfg->setValue(QStringLiteral("skipStillerThan"), A->skipStillerVal);
      cfg->setValue(QStringLiteral("transposition"), A->transposition);
#if !defined (Q_OS_ANDROID)
      cfg->setValue(QLatin1String("dumpPath"), A->dumpPath);
#endif
  cfg->endGroup();

  cfg->beginGroup(QLatin1String("layout"));
      cfg->setValue(QStringLiteral("toolBarAutoHide"), L->toolBarAutoHide);
      cfg->setValue(QStringLiteral("iconTextOnToolBar"), (int)L->iconTextOnToolBar);
      cfg->setValue(QStringLiteral("hintsBarEnabled"), L->hintsBarEnabled);
      cfg->setValue(QStringLiteral("soundViewEnabled"), L->soundViewEnabled);
      cfg->setValue(QStringLiteral("guitarEnabled"), L->guitarEnabled);
  cfg->endGroup();

//   cfg->beginGroup(QLatin1String("touch"));
//     cfg->setValue(QStringLiteral("scoreWasTouched"), TtouchParams::i()->scoreWasTouched);
//     cfg->setValue(QStringLiteral("clefWasTouched"), TtouchParams::i()->clefWasTouched);
//     cfg->setValue(QStringLiteral("guitarWasTouched"), TtouchParams::i()->guitarWasTouched);
//     cfg->setValue(QStringLiteral("initialAnimAccepted"), TtouchParams::i()->initialAnimAccepted);
//   cfg->endGroup();
}
