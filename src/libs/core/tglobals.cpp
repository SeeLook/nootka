/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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

QString Tglobals::getInstPath(const QString& appInstPath) {
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

QString Tglobals::systemUserName() {
  #if defined (Q_OS_ANDROID)
    return Tandroid::accountName();
  #elif defined(Q_OS_WIN32)
    return qgetenv("USERNAME");
  #else
    return qgetenv("USER");
  #endif
}

qreal Tglobals::pitchOfFreq(int freq) {
  return -36.3763165622959152488 + 39.8631371386483481 * log10(static_cast<qreal>(freq));
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
    QCoreApplication::setApplicationName(QStringLiteral("Nootka"));

  E = new TexamParams();
  A = new TaudioParams();
  S = new TscoreParams();
  L = new TlayoutParams();
  m_tuneObject = new TtuneObject(this);

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
  config = new QSettings(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("Nootka"), qApp->applicationName());
#else
  config = new QSettings();
#endif
  loadSettings(config);

  if (!m_instance) {
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
  m_instance = nullptr;
}


//##########################################################################################
//#######################         PUBLIC         ###########################################
//##########################################################################################

void Tglobals::setUseAnimations(bool use) {
  if (m_useAnimations != use) {
    m_useAnimations = use;
    emit useAnimationsChanged();
  }
}

void Tglobals::setShowHints(bool showH) {
  if (showH != m_showHints) {
    m_showHints = showH;
    emit showHintsChanged();
  }
}

void Tglobals::setGuiScale(qreal sc) {
  if (sc != m_guiScale) {
    m_guiScale = sc;
    emit guiScaleChanged();
  }
}


bool Tglobals::showEnharmNotes() const { return S->showEnharmNotes; }
void Tglobals::setShowEnharmNotes(bool showEnharm) {
  if (showEnharm != S->showEnharmNotes) {
    S->showEnharmNotes = showEnharm;
    emit showEnharmNotesChanged();
  }
}

QColor Tglobals::getEnharmNoteColor() const { return S->enharmNotesColor; }
void Tglobals::setEnharmNoteColor(const QColor& c) { S->enharmNotesColor = c; }

QColor Tglobals::getNoteCursorColor() const { return S->pointerColor; }
void Tglobals::setNoteCursorColor(const QColor& c) { S->pointerColor = c; emit noteCursorColorChanged(); }

bool Tglobals::isSingleNote() const { return S->isSingleNoteMode; }
void Tglobals::setSingleNote(bool sn) {
  if (sn != S->isSingleNoteMode) {
    S->isSingleNoteMode = sn;
    emit singleNoteModeChanged();
  }
}

bool Tglobals::enableDoubleAccids() const { return S->doubleAccidentalsEnabled; }
void Tglobals::setEnableDoubleAccids(bool dblAcc) {
  if (dblAcc != S->doubleAccidentalsEnabled) {
    S->doubleAccidentalsEnabled = dblAcc;
    emit enableDoubleAccidsChanged();
  }
}

bool Tglobals::keySignatureEnabled() const { return S->keySignatureEnabled; }
void Tglobals::setKeySignatureEnabled(bool enKey) {
  if (enKey != S->keySignatureEnabled) {
    S->keySignatureEnabled = enKey;
    emit enableKeySignatureChanged();
  }
}

int Tglobals::clefType() const { return static_cast<int>(S->clef); }
void Tglobals::setClefType(int clType) {
  if (static_cast<Tclef::EclefType>(clType) != S->clef) {
    S->clef = static_cast<Tclef::EclefType>(clType);
    emit clefTypeChanged();
  }
}

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

bool Tglobals::scientificOctaves() const { return S->scientificOctaves; }

void Tglobals::setScientificOctaves(bool sciO) {
  if (sciO != S->scientificOctaves) {
    S->scientificOctaves = sciO;
    Tnote::scientificOctaves = sciO;
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
    if (ok && frNr > 0 && static_cast<uint>(frNr) <= GfretsNumber)
      GmarkedFrets << fr[i] + exMark;
  }
}


/* ------------------ Sound switches ------------------ */
bool Tglobals::audioInEnabled() const { return A->INenabled; }
void Tglobals::setAudioInEnabled(bool inEnabled) { A->INenabled = inEnabled; }

QString Tglobals::inDevName() const { return A->INdevName; }
void Tglobals::setInDevName(const QString& inName) { A->INdevName = inName; }

int Tglobals::audioInstrument() const { return A->audioInstrNr; }
void Tglobals::setAudioInstrument(int ai) { A->audioInstrNr = ai; }

qreal Tglobals::minDuration() const { return A->minDuration; }

void Tglobals::setMinDuration(qreal md) { A->minDuration = md; }

qreal Tglobals::minVolume() const { return A->minimalVol; }
void Tglobals::setMinVolume(qreal mv) {
  if (mv != A->minimalVol) {
    A->minimalVol = mv;
    emit minVolumeChanged();
  }
}

int Tglobals::detectionMethod() const { return A->detectMethod; }
void Tglobals::setDetectionMethod(int m) { A->detectMethod = m; }

bool Tglobals::useFilter() const { return A->equalLoudness; }
void Tglobals::setUseFilter(bool use) { A->equalLoudness = use; }

bool Tglobals::audioOutEnabled() const { return A->OUTenabled; }
void Tglobals::setAudioOutEnabled(bool outEnabled) { A->OUTenabled = outEnabled; }

QString Tglobals::outDevName() const { return A->OUTdevName; }
void Tglobals::setOutDevName(const QString& odn) { A->OUTdevName = odn; }

bool Tglobals::forwardInput() const { return A->forwardInput; }
void Tglobals::setForwardInput(bool fi) { A->forwardInput = fi; }


int Tglobals::midAfreq() const { return A->midAfreq; }
void Tglobals::setMidAfreq(int midA) {
  if (midA != A->midAfreq) {
    if (midA < 391 || midA > 493) {
      qDebug() << "[Tglobals] middle A frequency out of supported range. Revert to 440Hz" ;
      midA = 440;
    }
    A->midAfreq = midA; // in range of two semitones up and down around middle A (440Hz)
    A->a440diff = midA == 440 ? 0.0 : pitchOfFreq(A->midAfreq) - pitchOfFreq(440);
    emit midAfreqChanged();
  }
}

bool Tglobals::JACKorASIO() const { return A->JACKorASIO; }
void Tglobals::setJACKorASIO(bool JorA) { A->JACKorASIO = JorA; }


/* ------------------ Exam switches ------------------ */
void Tglobals::setIsExam(bool is) {
  if (is != m_isExam) {
    m_isExam = is;
    emit isExamChanged();
  }
}

QString Tglobals::student() const { return E->studentName; }
void Tglobals::setStudent(const QString& st) {
  if (st != E->studentName) {
    E->studentName = st;
    emit studentChanged();
  }
}

void Tglobals::setCorrectColor(const QColor& c) {
  if (c != EanswerColor) {
    EanswerColor = c;
    emit correctColorChanged();
  }
}

void Tglobals::setNotBadColor(const QColor& c) {
  if (c != EnotBadColor) {
    EnotBadColor = c;
    emit notBadColorChanged();
  }
}

void Tglobals::setWrongColor(const QColor& c) {
  if (c != EquestionColor) {
    EquestionColor = c;
    emit wrongColorChanged();
  }
}

bool Tglobals::autoNextQuestion() const { return E->autoNextQuest; }
void Tglobals::setAutoNextQuestion(bool autoNext) { E->autoNextQuest = autoNext; }

bool Tglobals::expertAnswers() const { return E->expertsAnswerEnable; }
void Tglobals::setExpertAnswers(bool expertA) { E->expertsAnswerEnable = expertA; }

bool Tglobals::correctAnswers() const { return E->showCorrected; }
void Tglobals::setCorrectAnswers(bool corrAnsw) { E->showCorrected = corrAnsw; }

bool Tglobals::repeatIncorect() const { return E->repeatIncorrect; }
void Tglobals::setRepeatIncorect(bool repInCorr) { E->repeatIncorrect = repInCorr; }

bool Tglobals::closeWithoutConfirm() const { return E->closeWithoutConfirm; }
void Tglobals::setCloseWithoutConfirm(bool closeNoConf) { E->closeWithoutConfirm = closeNoConf; }

bool Tglobals::suggestExam() const { return E->suggestExam; }
void Tglobals::setSuggestExam(bool suggest) { E->suggestExam = suggest; }

bool Tglobals::extraNames() const { return E->showNameOfAnswered; }
void Tglobals::setExtraNames(bool extraN) { E->showNameOfAnswered = extraN; }

bool Tglobals::showWrongPlayed() const { return E->showWrongPlayed; }
void Tglobals::setShowWrongPlayed(bool wrongPlayed) { E->showWrongPlayed = wrongPlayed; }

bool Tglobals::waitForCorrect() const { return E->waitForCorrect; }
void Tglobals::setWaitForCorrect(bool waitFor) { E->waitForCorrect = waitFor; }

QString Tglobals::examsDir() const { return E->examsDir; }


void Tglobals::setGuitarParams(int fretNr, const Ttune& tun) {
  if (static_cast<uint>(fretNr) != GfretsNumber)
    GfretsNumber = fretNr;

  if (tun.type() == Ttune::Custom || tun.type() != tuning()->typeInt())
    setTune(tun);

  emit guitarParamsChanged();
}


int Tglobals::stringNumber() { return m_tune->stringNr(); }


void Tglobals::setFingerColor(const QColor& fc) { GfingerColor = fc; emit fingerColorChanged(); }
void Tglobals::setSelectedColor(const QColor& sc) { GselectedColor = sc; emit selectedColorChanged(); }


void Tglobals::loadSettings(QSettings* cfg) {
  // In fact, values without group are stored under 'General' key, but using it explicitly makes group '%General' - different.
  // It is messy, so get rid of directly calling that group
  if (cfg->contains(QLatin1String("General/geometry"))) { // old config key
      m_geometry = cfg->value(QStringLiteral("General/geometry"), QRect()).toRect();
      cfg->remove(QLatin1String("General/geometry")); // and remove it to grab new one next launch
  } else
      m_geometry = cfg->value(QStringLiteral("geometry"), QRect()).toRect();
  if (m_geometry.width() < 720 || m_geometry.height() < 480) {
    m_geometry.setWidth(qMax(qRound(qApp->primaryScreen()->size().width() * 0.75), 720));
    m_geometry.setHeight(qMax(qRound(qApp->primaryScreen()->size().height() * 0.75), 480));
    m_geometry.setX((qApp->primaryScreen()->size().width() - m_geometry.width()) / 2);
    m_geometry.setY((qApp->primaryScreen()->size().height() - m_geometry.height()) / 2);
  }
  m_guiScale = qBound(0.5, cfg->value(QStringLiteral("scale"), 1.0).toReal(), 1.5);

  cfg->beginGroup(QLatin1String("common"));
      isFirstRun = cfg->value(QStringLiteral("isFirstRun"), true).toBool();
      m_useAnimations = cfg->value(QStringLiteral("useAnimations"), true).toBool();
      m_showHints = cfg->value(QStringLiteral("showHints"), true).toBool();
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
      //TODO convert dropped bass clef
      // Rhythms has to be enabled when no clef (percussion)
      S->rhythmsEnabled = cfg->value(QStringLiteral("rhythmsEnabled"), true).toBool() || S->clef == Tclef::NoClef;
      S->isSingleNoteMode = cfg->value(QStringLiteral("singleNoteMode"), false).toBool();
      S->tempo = cfg->value(QStringLiteral("tempo"), 120).toInt();
      S->scoreScale = cfg->value(QStringLiteral("scoreScale"), 1.0).toReal();
      S->lastXmlDir = cfg->value(QStringLiteral("lastXmlDir"), QDir::homePath()).toString();
      S->scientificOctaves = cfg->value(QStringLiteral("scientificOctaves"), false).toBool();
      Tnote::scientificOctaves = S->scientificOctaves;
#if defined (Q_OS_ANDROID)
      S->lastXmlDir = cfg->value(QStringLiteral("lastXmlDir"), Tandroid::getExternalPath()).toString();
      if (!QFileInfo::exists(S->lastXmlDir)) // reset if doesn't exist
        S->lastXmlDir = Tandroid::getExternalPath();
#endif
  cfg->endGroup();

//common for score widget and note name
  cfg->beginGroup(QLatin1String("common"));
      S->doubleAccidentalsEnabled = cfg->value(QStringLiteral("doubleAccidentals"), false).toBool();
      S->showEnharmNotes = cfg->value(QStringLiteral("showEnaharmonicNotes"), false).toBool();
//       if (!S->isSingleNoteMode) // enharmonically equal notes can be enabled only in single note mode
//           S->showEnharmNotes = false;
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
          GfingerColor = QColor(255, 0, 127, 150); // nice pink with translucency
      if (cfg->contains("selectedColor"))
          GselectedColor = cfg->value(QStringLiteral("selectedColor")).value<QColor>();
      else
          GselectedColor = QColor(51, 153, 255); // nice blue as default
      auto tun = cfg->value(QStringLiteral("tune"));
      if (tun.isValid()) {
          Ttune tmpTune = tun.value<Ttune>();
          setTune(tmpTune);
      } else
          setTune(Ttune::stdTune);
      GpreferFlats = cfg->value(QStringLiteral("flatsPrefered"), false).toBool();;
      QList<QVariant> fretsList;
      fretsList << 5 << 7 << 9 << "12!" << 15 << 17;
      GmarkedFrets = cfg->value(QStringLiteral("dotsOnFrets"), fretsList).toList();
  cfg->endGroup();

// Exam settings
  cfg->beginGroup(QLatin1String("exam"));
      if (cfg->contains(QLatin1String("questionColor")))
        EquestionColor = cfg->value(QStringLiteral("questionColor")).value<QColor>();
      else
        EquestionColor = QColor(255, 0, 0); // red
      if (cfg->contains(QLatin1String("answerColor")))
        EanswerColor = cfg->value(QStringLiteral("answerColor")).value<QColor>();
      else
        EanswerColor = QColor(0, 255, 0); // green
      if (cfg->contains(QLatin1String("notBadColor")))
        EnotBadColor = cfg->value(QStringLiteral("notBadColor")).value<QColor>();
      else
        EnotBadColor = QColor(255, 128, 0); // #FF8000
      EquestionColor.setAlpha(255);
      EanswerColor.setAlpha(255);
      EnotBadColor.setAlpha(255);
      E->autoNextQuest = cfg->value(QStringLiteral("autoNextQuest"), false).toBool();
      E->repeatIncorrect = cfg->value(QStringLiteral("repeatIncorrect"), true).toBool();
      E->expertsAnswerEnable = cfg->value(QStringLiteral("expertsAnswerEnable"), false).toBool();
      E->studentName = cfg->value(QStringLiteral("studentName"), QString()).toString();
      if (E->studentName.isEmpty())
        E->studentName = systemUserName();
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
      E->waitForCorrect = cfg->value(QStringLiteral("waitForCorrect"), true).toBool();
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
    A->minimalVol = cfg->value(QStringLiteral("minimalVolume"), 0.4).toReal();
    A->dumpPath = cfg->value(QLatin1String("dumpPath"), QString()).toString();
#endif
    A->minDuration = cfg->value(QStringLiteral("minimalDuration"), 0.15).toReal(); // 150 ms
    setMidAfreq(cfg->value(QStringLiteral("midAfreq"), 440).toInt());
    A->intonation = static_cast<quint8>(qBound(0, cfg->value(QStringLiteral("intonation"), 3).toInt(), 5));
    A->forwardInput = cfg->value(QStringLiteral("forwardInput"), false).toBool();
    A->equalLoudness = cfg->value(QStringLiteral("equalLoudness"), true).toBool();
    A->transposition = cfg->value(QStringLiteral("transposition"), 0).toInt();
    A->stoppedByUser = cfg->value(QStringLiteral("stoppedByUser"), false).toBool();
    A->audibleMetro = cfg->value(QStringLiteral("audibleMetro"), false).toBool();
    A->countBefore = cfg->value(QStringLiteral("countBefore"), false).toBool();
    A->quantization = cfg->value(QStringLiteral("quantization"), 6).toInt();
  cfg->endGroup();

#if defined (Q_OS_ANDROID)
  m_keepScreenOn = cfg->value(QStringLiteral("keepScreenOn"), true).toBool();
  m_disableRotation = cfg->value(QStringLiteral("disableRotation"), true).toBool();
  m_fullScreen = cfg->value(QStringLiteral("fullScreen"), true).toBool();
  m_touchStopsSniff = cfg->value(QStringLiteral("touchStopsSniff"), true).toBool();
#endif

  bool transposeTuning = S->clef == Tclef::Bass_F_8down;
  if (S->clef == Tclef::Bass_F_8down) {
    // This is clear: old Nootka had dropped down bass clef, now there is just bass clef
    // so clef is changed and transposition is added
    qDebug() << "[Tglobals] Dropped bass clef detected. Converting to ordinary bass clef.";
    S->clef = Tclef::Bass_F;
    if (A->transposition == 0) {
      qDebug() << "[Tglobals] Adding sound transposition one octave down due to bass clef conversion.";
      A->transposition = -12;
    }
  }
  // But in-between versions (1.5 to 1.7.0) bass clef might have low notes and so tuning.
  // -12 is note B in contra octave (-2) and it is lowest note achieve on the Nootka score
  // So try to fix that here, with hope there are no false positives
  if (transposeTuning || (S->clef != Tclef::Tenor_C && loNote().chromatic() < -12)) {
    qDebug() << "[Tglobals] Tuning transposed one octave up to fit score capability.";
    m_tune->riseOctaveUp(); // As long as we are transposing all strings the same step, string order doesn't change.
  }

// TODO cleanup code below
//   cfg->beginGroup(QLatin1String("layout"));
//     L->guitarEnabled = cfg->value(QStringLiteral("guitarEnabled"), true).toBool();
// #if defined (Q_OS_ANDROID)
//     L->soundViewEnabled = cfg->value(QStringLiteral("soundViewEnabled"), false).toBool();
//   // override some options not supported under mobile systems
//   L->toolBarAutoHide = true;
//   L->iconTextOnToolBar = Qt::ToolButtonTextBesideIcon;
//   L->hintsBarEnabled = false;
//   GisRightHanded = true;
// #else
//     L->soundViewEnabled = cfg->value(QStringLiteral("soundViewEnabled"), true).toBool();
//     L->toolBarAutoHide = cfg->value(QStringLiteral("toolBarAutoHide"), false).toBool();
//     L->iconTextOnToolBar = Qt::ToolButtonStyle(cfg->value(QStringLiteral("iconTextOnToolBar"), 3).toInt());
//     L->hintsBarEnabled = cfg->value(QStringLiteral("hintsBarEnabled"), true).toBool();
// #endif
//   cfg->endGroup();

//   cfg->beginGroup(QLatin1String("touch"));
//     TtouchParams::i()->scoreWasTouched = cfg->value(QStringLiteral("scoreWasTouched"), false).toBool();
//     TtouchParams::i()->clefWasTouched = cfg->value(QStringLiteral("clefWasTouched"), false).toBool();
//     TtouchParams::i()->guitarWasTouched = cfg->value(QStringLiteral("guitarWasTouched"), false).toBool();
//     TtouchParams::i()->initialAnimAccepted = cfg->value(QStringLiteral("initialAnimAccepted"), false).toBool();
//   cfg->endGroup();
}


void Tglobals::setTune(const Ttune& t) {
  delete m_tune;
  m_tune = new Ttune(t.name, t.str(1), t.str(2), t.str(3), t.str(4), t.str(5), t.str(6), t.type());
  m_tuneObject->setTune(m_tune);
  // creating array with guitar strings ordered by their pitch height
  char openStr[6];
  for (int i = 0; i < 6; i++) {
    m_order[i] = i;
    if (m_tune->str(i + 1).note() != 0)
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


QString Tglobals::lastXmlDir() const { return S->lastXmlDir; }
void Tglobals::setLastXmlDir(const QString& lastXml) { S->lastXmlDir = lastXml; }



void Tglobals::storeSettings(QSettings* cfg) {
  cfg->setValue(QStringLiteral("geometry"), m_geometry);
  cfg->setValue(QStringLiteral("scale"), m_guiScale);

  cfg->beginGroup(QLatin1String("common"));
      cfg->setValue(QStringLiteral("isFirstRun"), isFirstRun);
      cfg->setValue(QStringLiteral("useAnimations"), m_useAnimations);
      cfg->setValue(QStringLiteral("showHints"), m_showHints);
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
      cfg->setValue(QStringLiteral("lastXmlDir"), S->lastXmlDir);
      cfg->setValue(QStringLiteral("scientificOctaves"), S->scientificOctaves);
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
      cfg->setValue(QStringLiteral("tune"), QVariant::fromValue(*Gtune()));
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
      cfg->setValue(QStringLiteral("waitForCorrect"), E->waitForCorrect);
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
      cfg->setValue(QStringLiteral("midAfreq"), A->midAfreq);
      cfg->setValue(QStringLiteral("intonation"), A->intonation);
      cfg->setValue(QStringLiteral("forwardInput"), A->forwardInput);
      cfg->setValue(QStringLiteral("equalLoudness"), A->equalLoudness);
      cfg->setValue(QStringLiteral("transposition"), A->transposition);
      cfg->setValue(QStringLiteral("stoppedByUser"), A->stoppedByUser);
      cfg->setValue(QStringLiteral("audibleMetro"), A->audibleMetro);
      cfg->setValue(QStringLiteral("countBefore"), A->countBefore);
      cfg->setValue(QStringLiteral("quantization"), A->quantization);
#if !defined (Q_OS_ANDROID)
      cfg->setValue(QLatin1String("dumpPath"), A->dumpPath);
#endif
  cfg->endGroup();

#if defined (Q_OS_ANDROID)
  cfg->setValue(QStringLiteral("keepScreenOn"), m_keepScreenOn);
  cfg->setValue(QStringLiteral("disableRotation"), m_disableRotation);
  cfg->setValue(QStringLiteral("fullScreen"), m_fullScreen);
  cfg->setValue(QStringLiteral("touchStopsSniff"), m_touchStopsSniff);
#endif

//   cfg->beginGroup(QLatin1String("layout"));
//       cfg->setValue(QStringLiteral("toolBarAutoHide"), L->toolBarAutoHide);
//       cfg->setValue(QStringLiteral("iconTextOnToolBar"), (int)L->iconTextOnToolBar);
//       cfg->setValue(QStringLiteral("hintsBarEnabled"), L->hintsBarEnabled);
//       cfg->setValue(QStringLiteral("soundViewEnabled"), L->soundViewEnabled);
//       cfg->setValue(QStringLiteral("guitarEnabled"), L->guitarEnabled);
//   cfg->endGroup();

//   cfg->beginGroup(QLatin1String("touch"));
//     cfg->setValue(QStringLiteral("scoreWasTouched"), TtouchParams::i()->scoreWasTouched);
//     cfg->setValue(QStringLiteral("clefWasTouched"), TtouchParams::i()->clefWasTouched);
//     cfg->setValue(QStringLiteral("guitarWasTouched"), TtouchParams::i()->guitarWasTouched);
//     cfg->setValue(QStringLiteral("initialAnimAccepted"), TtouchParams::i()->initialAnimAccepted);
//   cfg->endGroup();
}


#if defined (Q_OS_ANDROID)

void Tglobals::setDisableRotation(bool disRot) {
  if (disRot != m_disableRotation) {
    Tandroid::disableRotation(disRot);
    m_disableRotation = disRot;
  }
}


void Tglobals::keepScreenOn(bool on) {
  if (on != m_keepScreenOn) {
    Tandroid::keepScreenOn(on);
    m_keepScreenOn = on;
  }
}

#endif

