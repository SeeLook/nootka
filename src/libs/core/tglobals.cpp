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
#include "tcolor.h"
#include "tinitcorelib.h"
#include "tlayoutparams.h"
#include "tpath.h"
#include <music/tkeysignature.h>
#include <music/tnamestylefilter.h>
#include <music/ttuneobject.h>
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
#if defined(Q_OS_ANDROID)
#include <Android/tandroid.h>
#endif
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#include <QtCore/qsettings.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtGui/qscreen.h>

/*static*/
QString &Tglobals::path = Tpath::main;
Tglobals *Tglobals::m_instance = nullptr;

QString Tglobals::getInstPath(const QString &appInstPath)
{
    QString p;
    QDir d = QDir(appInstPath);
#if defined(Q_OS_WIN)
    p = d.path() + QLatin1String("/"); //  Windows
#elif defined(Q_OS_ANDROID)
    p = ":/";
#elif defined(Q_OS_LINUX)
    d.cdUp();
    p = d.path() + QLatin1String("/share/nootka/"); // Linux
#elif defined(Q_OS_MAC)
    d.cdUp();
    p = d.path() + QLatin1String("/Resources/"); // MacOs
#endif

    return p;
}

QString Tglobals::systemUserName()
{
#if defined(Q_OS_ANDROID)
    return Tandroid::accountName();
#elif defined(Q_OS_WIN32)
    return qgetenv("USERNAME");
#else
    return qgetenv("USER");
#endif
}

qreal Tglobals::pitchOfFreq(int freq)
{
    return -36.3763165622959152488 + 39.8631371386483481 * log10(static_cast<qreal>(freq));
}

/*end static*/

Tglobals::Tglobals(QObject *parent)
    : QObject(parent)
    , scoreParams(new TscoreParams)
    , examParams(new TexamParams)
    , audioParams(new TaudioParams)
    , layoutParams(new TlayoutParams)
    , m_tune(nullptr)
{
    version = NOOTKA_VERSION;

    qRegisterMetaTypeStreamOperators<Ttune>("Ttune");
    qRegisterMetaType<Tnote>("Tnote");

    QCoreApplication::setOrganizationName(QStringLiteral("Nootka"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("nootka.sf.net"));
    QCoreApplication::setApplicationName(QStringLiteral("Nootka"));

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
        qApp->exit(109);
    }
}

Tglobals::~Tglobals()
{
    storeSettings(config);
    delete examParams;
    delete audioParams;
    delete scoreParams;
    delete layoutParams;
    delete m_tune;
    delete config;
    m_instance = nullptr;
}

// ##########################################################################################
// #######################         PUBLIC         ###########################################
// ##########################################################################################

bool Tglobals::wasFirstRun() const
{
    return config->value(QStringLiteral("common/isFirstRun"), true).toBool();
}

void Tglobals::warnAboutNewerVersion(const QString &fileName)
{
    emit newerVersion(fileName); // TnootkaQML will handle that
}

void Tglobals::setUseAnimations(bool use)
{
    if (m_useAnimations != use) {
        m_useAnimations = use;
        emit useAnimationsChanged();
    }
}

void Tglobals::setShowHints(bool showH)
{
    if (showH != m_showHints) {
        m_showHints = showH;
        emit showHintsChanged();
    }
}

void Tglobals::setGuiScale(qreal sc)
{
    if (sc != m_guiScale) {
        m_guiScale = sc;
        emit guiScaleChanged();
    }
}

bool Tglobals::showEnharmNotes() const
{
    return scoreParams->showEnharmNotes;
}
void Tglobals::setShowEnharmNotes(bool showEnharm)
{
    if (showEnharm != scoreParams->showEnharmNotes) {
        scoreParams->showEnharmNotes = showEnharm;
        emit showEnharmNotesChanged();
    }
}

QColor Tglobals::getEnharmNoteColor() const
{
    return scoreParams->enharmNotesColor;
}
void Tglobals::setEnharmNoteColor(const QColor &c)
{
    scoreParams->enharmNotesColor = c;
}

QColor Tglobals::getNoteCursorColor() const
{
    return scoreParams->pointerColor;
}
void Tglobals::setNoteCursorColor(const QColor &c)
{
    scoreParams->pointerColor = c;
    emit noteCursorColorChanged();
}

bool Tglobals::isSingleNote() const
{
    return scoreParams->isSingleNoteMode;
}
void Tglobals::setSingleNote(bool sn)
{
    if (sn != scoreParams->isSingleNoteMode) {
        scoreParams->isSingleNoteMode = sn;
        emit singleNoteModeChanged();
    }
}

bool Tglobals::enableDoubleAccids() const
{
    return scoreParams->doubleAccidentalsEnabled;
}
void Tglobals::setEnableDoubleAccids(bool dblAcc)
{
    if (dblAcc != scoreParams->doubleAccidentalsEnabled) {
        scoreParams->doubleAccidentalsEnabled = dblAcc;
        emit enableDoubleAccidsChanged();
    }
}

bool Tglobals::keySignatureEnabled() const
{
    return scoreParams->keySignatureEnabled;
}
void Tglobals::setKeySignatureEnabled(bool enKey)
{
    if (enKey != scoreParams->keySignatureEnabled) {
        scoreParams->keySignatureEnabled = enKey;
        emit enableKeySignatureChanged();
    }
}

int Tglobals::clefType() const
{
    return static_cast<int>(scoreParams->clef);
}
void Tglobals::setClefType(int clType)
{
    if (static_cast<Tclef::EclefType>(clType) != scoreParams->clef) {
        scoreParams->clef = static_cast<Tclef::EclefType>(clType);
        emit clefTypeChanged();
    }
}

QString Tglobals::majorKeyNameSufix() const
{
    return scoreParams->majKeyNameSufix;
}
void Tglobals::setMajorKeyNameSufix(const QString &mkns)
{
    scoreParams->majKeyNameSufix = mkns;
}

QString Tglobals::minorKeyNameSufix() const
{
    return scoreParams->minKeyNameSufix;
}
void Tglobals::setMinorKeyNameSufix(const QString &mkns)
{
    scoreParams->minKeyNameSufix = mkns;
}

int Tglobals::keyNameStyle() const
{
    return static_cast<int>(scoreParams->nameStyleInKeySign);
}
void Tglobals::setKeyNameStyle(int keyStyle)
{
    scoreParams->nameStyleInKeySign = static_cast<Tnote::EnameStyle>(keyStyle);
}

bool Tglobals::showKeyName() const
{
    return scoreParams->showKeySignName;
}
void Tglobals::setShowKeyName(bool showKey)
{
    scoreParams->showKeySignName = showKey;
    emit showKeyNameChanged();
}

void Tglobals::updateKeySignatureNames()
{
    TkeySignature::setNameStyle(scoreParams->nameStyleInKeySign, scoreParams->majKeyNameSufix, scoreParams->minKeyNameSufix);
    emit keyNameChanged();
}

bool Tglobals::rhythmsEnabled() const
{
    return scoreParams->rhythmsEnabled;
}
void Tglobals::setRhythmsEnabled(bool enR)
{
    if (scoreParams->rhythmsEnabled != enR) {
        scoreParams->rhythmsEnabled = enR;
        emit rhythmsEnabledChanged();
    }
}

/* ------------------ Note name switches ------------------ */
bool Tglobals::seventhIsB() const
{
    return scoreParams->seventhIs_B;
}

void Tglobals::setSeventhIsB(bool isB)
{
    if (isB != scoreParams->seventhIs_B) {
        scoreParams->seventhIs_B = isB;
        emit seventhIsBChanged();
    }
}

int Tglobals::noteNameStyle() const
{
    return static_cast<int>(scoreParams->nameStyleInNoteName);
}

void Tglobals::setNoteNameStyle(int nameStyle)
{
    Tnote::EnameStyle newNameStyle = static_cast<Tnote::EnameStyle>(nameStyle);
    if (newNameStyle != scoreParams->nameStyleInNoteName) {
        scoreParams->nameStyleInNoteName = static_cast<Tnote::EnameStyle>(nameStyle);
        Tnote::defaultStyle = scoreParams->nameStyleInNoteName;
        emit noteNameStyleChanged();
    }
}

bool Tglobals::scientificOctaves() const
{
    return scoreParams->scientificOctaves;
}

void Tglobals::setScientificOctaves(bool sciO)
{
    if (sciO != scoreParams->scientificOctaves) {
        scoreParams->scientificOctaves = sciO;
        Tnote::scientificOctaves = sciO;
        emit noteNameStyleChanged();
    }
}

bool Tglobals::namesOnScore() const
{
    return scoreParams->namesOnScore;
}

void Tglobals::setNamesOnScore(bool showNames)
{
    if (showNames != scoreParams->namesOnScore) {
        scoreParams->namesOnScore = showNames;
        emit namesOnScoreChanged();
    }
}

QColor Tglobals::nameColor() const
{
    return scoreParams->nameColor;
}

void Tglobals::setNameColor(const QColor &nameC)
{
    if (nameC != scoreParams->nameColor) {
        scoreParams->nameColor = nameC;
        emit nameColorChanged();
    }
}

/* ------------------ Instrument switches ------------------ */
void Tglobals::setInstrument(Tinstrument::Etype t)
{
    if (t != m_instrument.type()) {
        m_instrument.setType(t);
        emit instrumentChanged();
    }
}

int Tglobals::transposition() const
{
    return audioParams->transposition;
}
void Tglobals::setTransposition(int t)
{
    if (t != audioParams->transposition) {
        audioParams->transposition = t;
        emit transpositionChanged();
    }
}

QString Tglobals::markedFrets() const
{
    QString fretText;
    for (int i = 0; i < GmarkedFrets.size(); ++i) {
        fretText.append(GmarkedFrets.at(i).toString());
        if (i < GmarkedFrets.size() - 1)
            fretText.append(QStringLiteral(","));
    }
    return fretText;
}

void Tglobals::setMarkedFrets(const QString &frets)
{
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
        if (ok && frNr > 0)
            GmarkedFrets << fr[i] + exMark;
    }
}

/* ------------------ Sound switches ------------------ */
bool Tglobals::audioInEnabled() const
{
    return audioParams->INenabled;
}
void Tglobals::setAudioInEnabled(bool inEnabled)
{
    audioParams->INenabled = inEnabled;
}

QString Tglobals::inDevName() const
{
    return audioParams->INdevName;
}
void Tglobals::setInDevName(const QString &inName)
{
    audioParams->INdevName = inName;
}

int Tglobals::audioInstrument() const
{
    return audioParams->audioInstrNr;
}
void Tglobals::setAudioInstrument(int ai)
{
    audioParams->audioInstrNr = ai;
}

qreal Tglobals::minDuration() const
{
    return audioParams->minDuration;
}

void Tglobals::setMinDuration(qreal md)
{
    audioParams->minDuration = md;
}

qreal Tglobals::minVolume() const
{
    return audioParams->minimalVol;
}
void Tglobals::setMinVolume(qreal mv)
{
    if (mv != audioParams->minimalVol) {
        audioParams->minimalVol = mv;
        emit minVolumeChanged();
    }
}

int Tglobals::detectionMethod() const
{
    return audioParams->detectMethod;
}
void Tglobals::setDetectionMethod(int m)
{
    audioParams->detectMethod = m;
}

bool Tglobals::useFilter() const
{
    return audioParams->equalLoudness;
}
void Tglobals::setUseFilter(bool use)
{
    audioParams->equalLoudness = use;
}

bool Tglobals::audioOutEnabled() const
{
    return audioParams->OUTenabled;
}
void Tglobals::setAudioOutEnabled(bool outEnabled)
{
    audioParams->OUTenabled = outEnabled;
}

QString Tglobals::outDevName() const
{
    return audioParams->OUTdevName;
}
void Tglobals::setOutDevName(const QString &odn)
{
    audioParams->OUTdevName = odn;
}

bool Tglobals::forwardInput() const
{
    return audioParams->forwardInput;
}
void Tglobals::setForwardInput(bool fi)
{
    audioParams->forwardInput = fi;
}

int Tglobals::midAfreq() const
{
    return audioParams->midAfreq;
}
void Tglobals::setMidAfreq(int midA)
{
    if (midA != audioParams->midAfreq) {
        if (midA < 391 || midA > 493) {
            qDebug() << "[Tglobals] middle A frequency out of supported range. Revert to 440Hz";
            midA = 440;
        }
        audioParams->midAfreq = midA; // in range of two semitones up and down around middle A (440Hz)
        audioParams->a440diff = midA == 440 ? 0.0 : pitchOfFreq(audioParams->midAfreq) - pitchOfFreq(440);
        emit midAfreqChanged();
    }
}

bool Tglobals::JACKorASIO() const
{
    return audioParams->JACKorASIO;
}
void Tglobals::setJACKorASIO(bool JorA)
{
    audioParams->JACKorASIO = JorA;
}

void Tglobals::setShowNotesDiff(bool notesDiff)
{
    if (m_showNotesDiff != notesDiff) {
        m_showNotesDiff = notesDiff;
        emit showNotesDiffChanged();
    }
}

/* ------------------ Exam switches ------------------ */
void Tglobals::setIsExam(bool is)
{
    if (is != m_isExam) {
        m_isExam = is;
        emit isExamChanged();
    }
}

QString Tglobals::student() const
{
    return examParams->studentName;
}
void Tglobals::setStudent(const QString &st)
{
    if (st != examParams->studentName) {
        examParams->studentName = st;
        emit studentChanged();
    }
}

void Tglobals::setCorrectColor(const QColor &c)
{
    if (c != EanswerColor) {
        EanswerColor = c;
        emit correctColorChanged();
    }
}

void Tglobals::setNotBadColor(const QColor &c)
{
    if (c != EnotBadColor) {
        EnotBadColor = c;
        emit notBadColorChanged();
    }
}

void Tglobals::setWrongColor(const QColor &c)
{
    if (c != EquestionColor) {
        EquestionColor = c;
        emit wrongColorChanged();
    }
}

bool Tglobals::autoNextQuestion() const
{
    return examParams->autoNextQuest;
}
void Tglobals::setAutoNextQuestion(bool autoNext)
{
    examParams->autoNextQuest = autoNext;
}

bool Tglobals::expertAnswers() const
{
    return examParams->expertsAnswerEnable;
}
void Tglobals::setExpertAnswers(bool expertA)
{
    examParams->expertsAnswerEnable = expertA;
}

bool Tglobals::correctAnswers() const
{
    return examParams->showCorrected;
}
void Tglobals::setCorrectAnswers(bool corrAnsw)
{
    examParams->showCorrected = corrAnsw;
}

bool Tglobals::repeatIncorect() const
{
    return examParams->repeatIncorrect;
}
void Tglobals::setRepeatIncorect(bool repInCorr)
{
    examParams->repeatIncorrect = repInCorr;
}

bool Tglobals::closeWithoutConfirm() const
{
    return examParams->closeWithoutConfirm;
}
void Tglobals::setCloseWithoutConfirm(bool closeNoConf)
{
    examParams->closeWithoutConfirm = closeNoConf;
}

bool Tglobals::suggestExam() const
{
    return examParams->suggestExam;
}
void Tglobals::setSuggestExam(bool suggest)
{
    examParams->suggestExam = suggest;
}

bool Tglobals::extraNames() const
{
    return examParams->showNameOfAnswered;
}
void Tglobals::setExtraNames(bool extraN)
{
    examParams->showNameOfAnswered = extraN;
}

bool Tglobals::showWrongPlayed() const
{
    return examParams->showWrongPlayed;
}
void Tglobals::setShowWrongPlayed(bool wrongPlayed)
{
    examParams->showWrongPlayed = wrongPlayed;
}

bool Tglobals::waitForCorrect() const
{
    return examParams->waitForCorrect;
}
void Tglobals::setWaitForCorrect(bool waitFor)
{
    examParams->waitForCorrect = waitFor;
}

QString Tglobals::examsDir() const
{
    return examParams->examsDir;
}

bool Tglobals::gotIt(const QString &key, bool retVal) const
{
    return config->value(QLatin1String("gotIt/") + key, retVal).toBool();
}

void Tglobals::setGotIt(const QString &key, bool val)
{
    config->setValue(QLatin1String("gotIt/") + key, val);
}

void Tglobals::setGuitarParams(int fretNr, const Ttune &tun)
{
    if (static_cast<uint>(fretNr) != GfretsNumber)
        GfretsNumber = fretNr;

    if (tun.type() == Ttune::Custom || tun.type() != tuning()->typeInt())
        setTune(tun);

    emit guitarParamsChanged();
}

int Tglobals::stringNumber()
{
    return m_tune->stringNr();
}

void Tglobals::setFingerColor(const QColor &fc)
{
    GfingerColor = fc;
    emit fingerColorChanged();
}
void Tglobals::setSelectedColor(const QColor &sc)
{
    GselectedColor = sc;
    emit selectedColorChanged();
}

void Tglobals::loadSettings(QSettings *cfg)
{
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

    // score widget settings
    cfg->beginGroup(QLatin1String("score"));
    scoreParams->keySignatureEnabled = cfg->value(QStringLiteral("keySignature"), false).toBool();
    scoreParams->showKeySignName = cfg->value(QStringLiteral("keyName"), true).toBool(); // true;
    scoreParams->nameStyleInKeySign = Tnote::EnameStyle(cfg->value(QStringLiteral("nameStyleInKey"), static_cast<int>(Tnote::e_english_Bb)).toInt());
    scoreParams->majKeyNameSufix = cfg->value(QStringLiteral("majorKeysSufix"), QString()).toString();
    scoreParams->minKeyNameSufix = cfg->value(QStringLiteral("minorKeysSufix"), QString()).toString();
    if (cfg->contains("pointerColor"))
        scoreParams->pointerColor = cfg->value(QStringLiteral("pointerColor")).value<QColor>();
    else
        scoreParams->pointerColor = Tcolor::invert(qApp->palette().highlight().color());
    scoreParams->clef = Tclef::EclefType(cfg->value(QStringLiteral("clef"), static_cast<int>(Tclef::Treble_G_8down)).toInt());
    // Rhythms has to be enabled when no clef (percussion)
    scoreParams->rhythmsEnabled = cfg->value(QStringLiteral("rhythmsEnabled"), true).toBool() || scoreParams->clef == Tclef::NoClef;
    scoreParams->isSingleNoteMode = cfg->value(QStringLiteral("singleNoteMode"), false).toBool();
    scoreParams->tempo = cfg->value(QStringLiteral("tempo"), 120).toInt();
    scoreParams->scoreScale = cfg->value(QStringLiteral("scoreScale"), 1.0).toReal();
#if defined(Q_OS_ANDROID)
    scoreParams->lastXmlDir = cfg->value(QStringLiteral("lastXmlDir"), QString()).toString();
    if (!scoreParams->lastXmlDir.isEmpty()
        && (scoreParams->lastXmlDir == QDir::homePath() || !QFileInfo::exists(scoreParams->lastXmlDir) || !QFileInfo(scoreParams->lastXmlDir).isWritable())) {
        scoreParams->lastXmlDir.clear();
        /** WORKAROUND: This is workaround for 2.0.0 bug where lastXmlDir was set to QDir::homePath()
         * which is internal application location - no way to navigate outside,
         * so musicXML files on device storage(s) couldn't be reached. */
        qDebug() << "[Tglobals] Fixed music XML directory path.";
    }
#else
    scoreParams->lastXmlDir = cfg->value(QStringLiteral("lastXmlDir"), QDir::homePath()).toString();
#endif
    scoreParams->scientificOctaves = cfg->value(QStringLiteral("scientificOctaves"), false).toBool();
    Tnote::scientificOctaves = scoreParams->scientificOctaves;
    cfg->endGroup();

    // common for score widget and note name
    cfg->beginGroup(QLatin1String("common"));
    scoreParams->doubleAccidentalsEnabled = cfg->value(QStringLiteral("doubleAccidentals"), false).toBool();
    scoreParams->showEnharmNotes = cfg->value(QStringLiteral("showEnaharmonicNotes"), false).toBool();
    //       if (!S->isSingleNoteMode) // enharmonically equal notes can be enabled only in single note mode
    //           S->showEnharmNotes = false;
    scoreParams->enharmNotesColor = cfg->value(QStringLiteral("enharmonicNotesColor"), QColor(0, 162, 162)).value<QColor>(); // turquoise
    scoreParams->seventhIs_B = cfg->value(QStringLiteral("is7thNote_B"), true).toBool(); // true;
    cfg->endGroup();

    // note name settings
    cfg->beginGroup(QLatin1String("noteName"));
    scoreParams->nameStyleInNoteName = Tnote::EnameStyle(cfg->value(QStringLiteral("nameStyle"), static_cast<int>(Tnote::e_english_Bb)).toInt());
    scoreParams->solfegeStyle = Tnote::EnameStyle(cfg->value(QStringLiteral("solfegeStyle"), static_cast<int>(getSolfegeStyle())).toInt());
    scoreParams->octaveInNoteNameFormat = cfg->value(QStringLiteral("octaveInName"), true).toBool();
    scoreParams->namesOnScore = cfg->value(QStringLiteral("namesOnScore"), true).toBool();
    scoreParams->nameColor = cfg->value(QStringLiteral("namesColor"), QColor(0, 225, 225)).value<QColor>();
    cfg->endGroup();
    // Fix name style depending on 7th note is was set wrongly in configuration (naughty user)
    if (scoreParams->seventhIs_B) {
        if (scoreParams->nameStyleInNoteName == Tnote::e_norsk_Hb)
            scoreParams->nameStyleInNoteName = Tnote::e_english_Bb;
        else if (scoreParams->nameStyleInNoteName == Tnote::e_deutsch_His)
            scoreParams->nameStyleInNoteName = Tnote::e_nederl_Bis;
    } else {
        if (scoreParams->nameStyleInNoteName == Tnote::e_english_Bb)
            scoreParams->nameStyleInNoteName = Tnote::e_norsk_Hb;
        else if (scoreParams->nameStyleInNoteName == Tnote::e_nederl_Bis)
            scoreParams->nameStyleInNoteName = Tnote::e_deutsch_His;
    }
    // Initialize name filter
    TnameStyleFilter::setStyleFilter(&scoreParams->seventhIs_B, &scoreParams->solfegeStyle);
    Tnote::defaultStyle = scoreParams->nameStyleInNoteName;
    // verify key name style - does it match with 7th note name
    auto tmpKeyStyle = scoreParams->nameStyleInKeySign;
    scoreParams->nameStyleInKeySign = TnameStyleFilter::get(scoreParams->nameStyleInKeySign);
    if (tmpKeyStyle != scoreParams->nameStyleInKeySign)
        qDebug() << "[Tglobals] Name style in key signatures" << tmpKeyStyle << "didn't match with 7th note name. Fixed to" << scoreParams->nameStyleInKeySign;

    // guitar settings
    Ttune::prepareDefinedTunes();
    cfg->beginGroup(QLatin1String("guitar"));
    int instr = cfg->value(QStringLiteral("instrument"), static_cast<int>(Tinstrument::ClassicalGuitar)).toInt();
    if (instr < 0 || instr >= INSTR_COUNT) {
        qDebug() << "[Tglobals] Unsupported instrument in configuration file!" << instr << "\nReseted to none.";
        instr = 0;
    }
    m_instrument.setType(static_cast<Tinstrument::Etype>(instr));
    GfretsNumber = cfg->value(QStringLiteral("fretNumber"), 19).toInt();
    GisRightHanded = cfg->value(QStringLiteral("rightHanded"), true).toBool(); // true;
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
    GpreferFlats = cfg->value(QStringLiteral("flatsPrefered"), false).toBool();
    ;
    QList<QVariant> fretsList;
    fretsList << 5 << 7 << (m_instrument.ukulele() ? 10 : 9) << "12!";
    if (!m_instrument.ukulele())
        fretsList << 15 << 17;
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
    examParams->autoNextQuest = cfg->value(QStringLiteral("autoNextQuest"), false).toBool();
    examParams->repeatIncorrect = cfg->value(QStringLiteral("repeatIncorrect"), true).toBool();
    examParams->expertsAnswerEnable = cfg->value(QStringLiteral("expertsAnswerEnable"), false).toBool();
    examParams->studentName = cfg->value(QStringLiteral("studentName"), QString()).toString();
    if (examParams->studentName.isEmpty())
        examParams->studentName = systemUserName();
#if defined(Q_OS_ANDROID)
    bool hasWriteAccess = Tandroid::hasWriteAccess();
    if (hasWriteAccess) {
        examParams->examsDir = cfg->value(QStringLiteral("examsDir"), Tandroid::getExternalPath()).toString();
        if (!QFileInfo::exists(examParams->examsDir) || !QFileInfo(examParams->examsDir).isWritable()) // reset if doesn't exist
            examParams->examsDir = Tandroid::getExternalPath();
        examParams->levelsDir = cfg->value(QStringLiteral("levelsDir"), Tandroid::getExternalPath()).toString();
        if (!QFileInfo::exists(examParams->levelsDir) || !QFileInfo(examParams->levelsDir).isWritable())
            examParams->levelsDir = Tandroid::getExternalPath();
    }
#else
    examParams->examsDir = cfg->value(QStringLiteral("examsDir"), QDir::homePath()).toString();
    if (!QFileInfo::exists(examParams->examsDir)) // reset if doesn't exist
        examParams->examsDir = QDir::homePath();
    examParams->levelsDir = cfg->value(QStringLiteral("levelsDir"), QDir::homePath()).toString();
    if (!QFileInfo::exists(examParams->levelsDir)) // reset if doesn't exist
        examParams->levelsDir = QDir::homePath();
#endif
    examParams->closeWithoutConfirm = cfg->value(QStringLiteral("closeWithoutConfirm"), false).toBool();
    examParams->showCorrected = cfg->value(QStringLiteral("showCorrected"), true).toBool();
    examParams->mistakePreview = cfg->value(QStringLiteral("mistakePreview"), 3000).toInt();
    examParams->correctPreview = cfg->value(QStringLiteral("correctPreview"), 3000).toInt();
    examParams->questionDelay = cfg->value(QStringLiteral("questionDelay"), 150).toInt();
    examParams->suggestExam = cfg->value(QStringLiteral("suggestExam"), true).toBool();
    examParams->afterMistake = (TexamParams::EafterMistake)cfg->value(QStringLiteral("afterMistake"), static_cast<int>(TexamParams::e_continue)).toInt();
    examParams->showNameOfAnswered = cfg->value(QStringLiteral("showNameOfAnswered"), true).toBool();
    examParams->showWrongPlayed = cfg->value(QStringLiteral("showWrongPlayed"), false).toBool();
    examParams->waitForCorrect = cfg->value(QStringLiteral("waitForCorrect"), true).toBool();
    examParams->showHelpOnStart = cfg->value(QStringLiteral("showHelpOnStart"), true).toBool();
    examParams->askAboutExpert = cfg->value(QStringLiteral("askAboutExpert"), true).toBool();
    cfg->endGroup();

    // Sound settings
    cfg->beginGroup(QLatin1String("sound"));
    audioParams->JACKorASIO = cfg->value(QStringLiteral("JACKorASIO"), false).toBool();
    audioParams->OUTenabled = cfg->value(QStringLiteral("outSoundEnabled"), true).toBool();
    audioParams->OUTdevName = cfg->value(QStringLiteral("outDeviceName"), QString()).toString();
    audioParams->midiEnabled = cfg->value(QStringLiteral("midiEnabled"), false).toBool();
    audioParams->midiPortName = cfg->value(QStringLiteral("midiPortName"), QString()).toString();
    audioParams->midiInstrNr = (unsigned char)cfg->value(QStringLiteral("midiInstrumentNr"), 0).toInt();
    audioParams->audioInstrNr = qBound(1, cfg->value(QStringLiteral("audioInstrumentNr"), 1).toInt(), INSTR_COUNT);
    audioParams->INenabled = cfg->value(QStringLiteral("inSoundEnabled"), true).toBool();
    audioParams->INdevName = cfg->value(QStringLiteral("inDeviceName"), QString()).toString();
    audioParams->detectMethod = qBound(0, cfg->value(QStringLiteral("detectionMethod"), 2).toInt(), 2); // MPM modified cepstrum
#if defined(Q_OS_ANDROID) // Input sound is loud on mobile
    audioParams->minimalVol = cfg->value(QStringLiteral("minimalVolume"), 0.6).toFloat();
#else
    audioParams->minimalVol = cfg->value(QStringLiteral("minimalVolume"), 0.4).toReal();
    audioParams->dumpPath = cfg->value(QLatin1String("dumpPath"), QString()).toString();
    m_showNotesDiff = cfg->value(QStringLiteral("showNotesDiff"), false).toBool();
#endif
    audioParams->minDuration = cfg->value(QStringLiteral("minimalDuration"), 0.15).toReal(); // 150 ms
    setMidAfreq(cfg->value(QStringLiteral("midAfreq"), 440).toInt());
    audioParams->intonation = static_cast<quint8>(qBound(0, cfg->value(QStringLiteral("intonation"), 3).toInt(), 5));
    audioParams->forwardInput = cfg->value(QStringLiteral("forwardInput"), false).toBool();
    audioParams->equalLoudness = cfg->value(QStringLiteral("equalLoudness"), true).toBool();
    audioParams->transposition = cfg->value(QStringLiteral("transposition"), 0).toInt();
    audioParams->stoppedByUser = cfg->value(QStringLiteral("stoppedByUser"), false).toBool();
    audioParams->audibleMetro = cfg->value(QStringLiteral("audibleMetro"), false).toBool();
    audioParams->countBefore = cfg->value(QStringLiteral("countBefore"), false).toBool();
    audioParams->quantization = cfg->value(QStringLiteral("quantization"), 6).toInt();
    cfg->endGroup();

#if defined(Q_OS_ANDROID)
    m_keepScreenOn = cfg->value(QStringLiteral("keepScreenOn"), true).toBool();
    m_disableRotation = cfg->value(QStringLiteral("disableRotation"), true).toBool();
    m_fullScreen = cfg->value(QStringLiteral("fullScreen"), true).toBool();
    m_touchStopsSniff = cfg->value(QStringLiteral("touchStopsSniff"), true).toBool();
#endif

    bool transposeTuning = scoreParams->clef == Tclef::Bass_F_8down;
    if (scoreParams->clef == Tclef::Bass_F_8down) {
        // This is clear: old Nootka had dropped down bass clef, now there is just bass clef
        // so clef is changed and transposition is added
        qDebug() << "[Tglobals] Dropped bass clef detected. Converting to ordinary bass clef.";
        scoreParams->clef = Tclef::Bass_F;
        if (audioParams->transposition == 0) {
            qDebug() << "[Tglobals] Adding sound transposition one octave down due to bass clef conversion.";
            audioParams->transposition = -12;
        }
    }
    // But in-between versions (1.5 to 1.7.0) bass clef might have low notes and so tuning.
    // -12 is note B in contra octave (-2) and it is lowest note achieve on the Nootka score
    // So try to fix that here, with hope there are no false positives
    if (transposeTuning || (scoreParams->clef != Tclef::Tenor_C && loNote().chromatic() < -12)) {
        qDebug() << "[Tglobals] Tuning transposed one octave up to fit score capability.";
        m_tune->riseOctaveUp(); // As long as we are transposing all strings the same step, string order doesn't change.
    }
}

void Tglobals::setTune(const Ttune &t)
{
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

Tnote Tglobals::hiString()
{
    return m_tune->str(m_order[0] + 1);
}

Tnote Tglobals::loString()
{
    return m_tune->str(m_order[m_tune->stringNr() - 1] + 1);
}

Tnote::EnameStyle Tglobals::getSolfegeStyle()
{
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

QString Tglobals::lastXmlDir() const
{
    return scoreParams->lastXmlDir;
}
void Tglobals::setLastXmlDir(const QString &lastXml)
{
    scoreParams->lastXmlDir = lastXml;
}

void Tglobals::storeSettings(QSettings *cfg)
{
    cfg->setValue(QStringLiteral("geometry"), m_geometry);
    cfg->setValue(QStringLiteral("scale"), m_guiScale);

    cfg->beginGroup(QLatin1String("common"));
    cfg->setValue(QStringLiteral("isFirstRun"), isFirstRun);
    cfg->setValue(QStringLiteral("useAnimations"), m_useAnimations);
    cfg->setValue(QStringLiteral("showHints"), m_showHints);
    cfg->setValue(QStringLiteral("doubleAccidentals"), scoreParams->doubleAccidentalsEnabled);
    cfg->setValue(QStringLiteral("showEnaharmonicNotes"), scoreParams->showEnharmNotes);
    cfg->setValue(QStringLiteral("enharmonicNotesColor"), scoreParams->enharmNotesColor);
    cfg->setValue(QStringLiteral("is7thNote_B"), scoreParams->seventhIs_B);
    cfg->setValue(QStringLiteral("language"), lang);
    cfg->endGroup();

    cfg->beginGroup(QLatin1String("score"));
    cfg->setValue(QStringLiteral("keySignature"), scoreParams->keySignatureEnabled);
    cfg->setValue(QStringLiteral("keyName"), scoreParams->showKeySignName);
    cfg->setValue(QStringLiteral("nameStyleInKey"), static_cast<int>(scoreParams->nameStyleInKeySign));
    QString majS, minS;
    if (scoreParams->majKeyNameSufix != TkeySignature::majorSufixTxt())
        majS = scoreParams->majKeyNameSufix;
    else
        majS.clear(); // default suffixes are reset to be translatable in next run
    cfg->setValue(QStringLiteral("majorKeysSufix"), majS);
    if (scoreParams->minKeyNameSufix != TkeySignature::minorSufixTxt())
        minS = scoreParams->minKeyNameSufix;
    else
        minS.clear();
    cfg->setValue(QStringLiteral("minorKeysSufix"), minS);
    cfg->setValue(QStringLiteral("pointerColor"), scoreParams->pointerColor);
    cfg->setValue(QStringLiteral("clef"), static_cast<int>(scoreParams->clef));
    cfg->setValue(QStringLiteral("rhythmsEnabled"), scoreParams->rhythmsEnabled);
    cfg->setValue(QStringLiteral("singleNoteMode"), scoreParams->isSingleNoteMode);
    cfg->setValue(QStringLiteral("tempo"), scoreParams->tempo);
    cfg->setValue(QStringLiteral("scoreScale"), scoreParams->scoreScale);
    cfg->setValue(QStringLiteral("lastXmlDir"), scoreParams->lastXmlDir);
    cfg->setValue(QStringLiteral("scientificOctaves"), scoreParams->scientificOctaves);
    cfg->endGroup();

    cfg->beginGroup(QLatin1String("noteName"));
    cfg->setValue(QStringLiteral("nameStyle"), static_cast<int>(scoreParams->nameStyleInNoteName));
    cfg->setValue(QStringLiteral("octaveInName"), scoreParams->octaveInNoteNameFormat);
    cfg->setValue(QStringLiteral("solfegeStyle"), scoreParams->solfegeStyle);
    cfg->setValue(QStringLiteral("namesOnScore"), scoreParams->namesOnScore);
    cfg->setValue(QStringLiteral("namesColor"), scoreParams->nameColor);
    cfg->endGroup();

    cfg->beginGroup(QLatin1String("guitar"));
    cfg->setValue(QStringLiteral("instrument"), static_cast<int>(m_instrument.type()));
    cfg->setValue(QStringLiteral("fretNumber"), static_cast<int>(GfretsNumber));
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
    cfg->setValue(QStringLiteral("autoNextQuest"), examParams->autoNextQuest);
    cfg->setValue(QStringLiteral("repeatIncorrect"), examParams->repeatIncorrect);
    cfg->setValue(QStringLiteral("showCorrected"), examParams->showCorrected);
    cfg->setValue(QStringLiteral("expertsAnswerEnable"), examParams->expertsAnswerEnable);
    cfg->setValue(QStringLiteral("studentName"), examParams->studentName);
    cfg->setValue(QStringLiteral("examsDir"), examParams->examsDir);
    cfg->setValue(QStringLiteral("levelsDir"), examParams->levelsDir);
    cfg->setValue(QStringLiteral("closeWithoutConfirm"), examParams->closeWithoutConfirm);
    cfg->setValue(QStringLiteral("mistakePreview"), examParams->mistakePreview);
    cfg->setValue(QStringLiteral("correctPreview"), examParams->correctPreview);
    cfg->setValue(QStringLiteral("questionDelay"), examParams->questionDelay);
    cfg->setValue(QStringLiteral("suggestExam"), examParams->suggestExam);
    cfg->setValue(QStringLiteral("afterMistake"), static_cast<int>(examParams->afterMistake));
    cfg->setValue(QStringLiteral("showNameOfAnswered"), examParams->showNameOfAnswered);
    cfg->setValue(QStringLiteral("showWrongPlayed"), examParams->showWrongPlayed);
    cfg->setValue(QStringLiteral("waitForCorrect"), examParams->waitForCorrect);
    cfg->setValue(QStringLiteral("askAboutExpert"), examParams->askAboutExpert);
    cfg->setValue(QStringLiteral("showHelpOnStart"), examParams->showHelpOnStart);
    cfg->endGroup();

    cfg->beginGroup(QLatin1String("sound"));
    cfg->setValue(QStringLiteral("JACKorASIO"), audioParams->JACKorASIO);
    cfg->setValue(QStringLiteral("outSoundEnabled"), audioParams->OUTenabled);
    cfg->setValue(QStringLiteral("outDeviceName"), audioParams->OUTdevName);
    cfg->setValue(QStringLiteral("midiEnabled"), audioParams->midiEnabled);
    cfg->setValue(QStringLiteral("midiPortName"), audioParams->midiPortName);
    cfg->setValue(QStringLiteral("midiInstrumentNr"), static_cast<int>(audioParams->midiInstrNr));
    cfg->setValue(QStringLiteral("audioInstrumentNr"), static_cast<int>(audioParams->audioInstrNr));
    cfg->setValue(QStringLiteral("inSoundEnabled"), audioParams->INenabled);
    cfg->setValue(QStringLiteral("inDeviceName"), audioParams->INdevName);
    cfg->setValue(QStringLiteral("detectionMethod"), audioParams->detectMethod);
    cfg->setValue(QStringLiteral("minimalVolume"), audioParams->minimalVol);
    cfg->setValue(QStringLiteral("minimalDuration"), audioParams->minDuration);
    cfg->setValue(QStringLiteral("midAfreq"), audioParams->midAfreq);
    cfg->setValue(QStringLiteral("intonation"), audioParams->intonation);
    cfg->setValue(QStringLiteral("forwardInput"), audioParams->forwardInput);
    cfg->setValue(QStringLiteral("equalLoudness"), audioParams->equalLoudness);
    cfg->setValue(QStringLiteral("transposition"), audioParams->transposition);
    cfg->setValue(QStringLiteral("stoppedByUser"), audioParams->stoppedByUser);
    cfg->setValue(QStringLiteral("audibleMetro"), audioParams->audibleMetro);
    cfg->setValue(QStringLiteral("countBefore"), audioParams->countBefore);
    cfg->setValue(QStringLiteral("quantization"), audioParams->quantization);
#if !defined(Q_OS_ANDROID)
    cfg->setValue(QStringLiteral("showNotesDiff"), m_showNotesDiff);
    cfg->setValue(QLatin1String("dumpPath"), audioParams->dumpPath);
#endif
    cfg->endGroup();

#if defined(Q_OS_ANDROID)
    cfg->setValue(QStringLiteral("keepScreenOn"), m_keepScreenOn);
    cfg->setValue(QStringLiteral("disableRotation"), m_disableRotation);
    cfg->setValue(QStringLiteral("fullScreen"), m_fullScreen);
    cfg->setValue(QStringLiteral("touchStopsSniff"), m_touchStopsSniff);
#endif
}

#if defined(Q_OS_ANDROID)

void Tglobals::setDisableRotation(bool disRot)
{
    if (disRot != m_disableRotation) {
        Tandroid::disableRotation(disRot);
        m_disableRotation = disRot;
    }
}

void Tglobals::keepScreenOn(bool on)
{
    if (on != m_keepScreenOn) {
        Tandroid::keepScreenOn(on);
        m_keepScreenOn = on;
    }
}

#endif
