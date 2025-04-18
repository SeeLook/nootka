/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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

#include "tlevelcreatoritem.h"
#include "tlevelpreviewitem.h"
#include "tlevelselector.h"
#include <Android/tfiledialog.h>
#include <exam/tlevel.h>
#include <music/ttune.h>
#include <texamparams.h>
#include <tglobals.h>
#if defined(Q_OS_ANDROID)
#include <Android/tandroid.h>
#endif

#include <QtCore/qdatetime.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>

TlevelCreatorItem::TlevelCreatorItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    m_level = new Tlevel();
    m_level->name.clear();
    m_level->desc.clear();
    m_title = tr("Level creator");
    m_answersList << m_level->answersAs[0].value() << m_level->answersAs[1].value() << m_level->answersAs[2].value() << m_level->answersAs[3].value();

    QTimer::singleShot(500, this, [=] {
        emit updateLevel();
    });
}

TlevelCreatorItem::~TlevelCreatorItem()
{
    delete m_level;
}

void TlevelCreatorItem::setSelector(TlevelSelector *sel)
{
    if (sel != m_selector) {
        m_selector = sel;
        if (m_selector)
            connect(m_selector, &TlevelSelector::levelChanged, this, &TlevelCreatorItem::whenLevelChanged);
    }
}

QString TlevelCreatorItem::title() const
{
    return m_title + m_titleExtension;
}

bool TlevelCreatorItem::notSaved() const
{
    return !m_titleExtension.isEmpty();
}

void TlevelCreatorItem::saveLevel()
{
    emit save();
    if (!m_level->canBeInstr() && !m_level->answerIsSound()) { // no guitar and no played sound
        // adjust fret range - validation will skip it for non guitar levels
        m_level->loFret = 0; // Set range to fret number and rest will be done by function preparing question list
        m_level->hiFret = GLOB->GfretsNumber;
        m_level->onlyLowPos = true; // otherwise the above invokes doubled/tripled questions in the list
        // set all strings as available
        for (int str = 0; str < 6; str++)
            m_level->usedStrings[str] = true;
    }
    auto validMessage = validateLevel();
    if (!validMessage.isEmpty()) {
        showValidationMessage(validMessage);
        return;
    }
    // set instrument to none when it is not important for the level
    m_level->instrument = m_level->detectInstrument(GLOB->instrument().type());
    // invoke QML routines
    bool isWrong = m_level->desc.contains(QStringLiteral("<font color=\"red\">"));
    // but do not display description of not suitable level
    emit saveNewLevel(m_level->name, isWrong ? QString() : m_level->desc.replace(QLatin1String("<br>"), QLatin1String("\n")));
}

void TlevelCreatorItem::continueLevelSave(const QString &name, const QString &desc)
{
    m_level->name = name;
    m_level->desc = desc;

#if defined(Q_OS_ANDROID)
    if (GLOB->examParams->levelsDir.isEmpty())
        GLOB->examParams->levelsDir = Tandroid::getExternalPath();
#endif
    // Saving to file
    QLatin1String dotNel(".nel");
    bool toTr = name.startsWith(QLatin1String("tr("));
    QString fName = QDir::toNativeSeparators(GLOB->examParams->levelsDir + QLatin1String("/") + m_level->name.mid(toTr ? 3 : 0));
    fName = fName.replace(QLatin1String("."), QString()); // HACK: file dialogues don't like dots in the names
    if (QFileInfo::exists(fName + dotNel))
        fName += QLatin1String("-") + QDateTime::currentDateTime().toString(QLatin1String("(dd-MMM-hhmmss)"));
#if defined(Q_OS_ANDROID)
    QString fileName = TfileDialog::getSaveFileName(fName, QStringLiteral("nel"));
#else
    QString fileName = TfileDialog::getSaveFileName(tr("Save exam level"), fName, TlevelSelector::levelFilterTxt() + QLatin1String(" (*.nel)"));
#endif
    if (fileName.isEmpty()) {
        qDebug() << "[TlevelCreatorItem] Empty file name!";
        if (m_resumeAfterLevelChange)
            resumeAfterLevelChange();
        return;
    }

    if (fileName.right(4) != dotNel)
        fileName += dotNel;
    GLOB->examParams->levelsDir = QFileInfo(fileName).absoluteDir().absolutePath();
    if (!Tlevel::saveToFile(*m_level, fileName)) {
        wantNotSavedMessage(QString(), tr("Cannot open file for writing") + QLatin1String("\n") + fileName);
        if (m_resumeAfterLevelChange)
            resumeAfterLevelChange();
        return;
    }

    m_titleExtension.clear();
    emit saveStateChanged();
    selector()->addLevel(*m_level, fileName, true);
    selector()->updateRecentLevels(); // Put the file name to the settings list
    emit selector() -> levelsModelChanged();
    emit selector() -> selectLast();
    selector()->showLevel(selector()->currentIndex()); // Refresh level preview after save - it could got another name and description

    if (m_resumeAfterLevelChange)
        resumeAfterLevelChange();
}

void TlevelCreatorItem::checkLevel()
{
    QStringList validMessage = validateLevel();
    if (validMessage.isEmpty())
        emit wantValidationMessage(tr("Level validation"), QStringList(tr("Level seems to be correct")), Qt::green);
    else
        showValidationMessage(validMessage);
}

// Questions page
int TlevelCreatorItem::questionAs() const
{
    return m_level->questionAs.value();
}
void TlevelCreatorItem::setQuestionAs(int qAs)
{
    bool prevState = m_level->questionAs.isOnScore();
    bool doEmit = false;
    m_level->questionAs.setOnScore(qAs & 1);
    if (m_level->questionAs.isOnScore() != prevState) {
        doEmit = true;
        if (m_level->questionAs.isOnScore()) { // question checkbox was set, so select all answers
            m_level->answersAs[TQAtype::e_onScore].setValue(15);
            m_answersList[TQAtype::e_onScore] = 15;
        } else { // question was unset, switch off all answers
            m_level->answersAs[TQAtype::e_onScore].setValue(0);
            m_answersList[TQAtype::e_onScore] = 0;
        }
    }
    prevState = m_level->questionAs.isName();
    m_level->questionAs.setAsName(qAs & 2);
    if (m_level->questionAs.isName() != prevState) {
        doEmit = true;
        if (m_level->questionAs.isName()) { // question checkbox was set, so select all answers
            m_level->answersAs[TQAtype::e_asName].setValue(15);
            m_answersList[TQAtype::e_asName] = 15;
        } else { // question was unset, switch off all answers
            m_level->answersAs[TQAtype::e_asName].setValue(0);
            m_answersList[TQAtype::e_asName] = 0;
        }
    }
    prevState = m_level->questionAs.isOnInstr();
    m_level->questionAs.setOnInstr(qAs & 4);
    if (m_level->questionAs.isOnInstr() != prevState) {
        doEmit = true;
        if (m_level->questionAs.isOnInstr()) { // question checkbox was set, so select all answers
            m_level->answersAs[TQAtype::e_onInstr].setValue(15);
            m_answersList[TQAtype::e_onInstr] = 15;
        } else { // question was unset, switch off all answers
            m_level->answersAs[TQAtype::e_onInstr].setValue(0);
            m_answersList[TQAtype::e_onInstr] = 0;
        }
    }
    prevState = m_level->questionAs.isSound();
    m_level->questionAs.setAsSound(qAs & 8);
    if (m_level->questionAs.isSound() != prevState) {
        doEmit = true;
        if (m_level->questionAs.isSound()) { // question checkbox was set, so select all answers
            m_level->answersAs[TQAtype::e_asSound].setValue(15);
            m_answersList[TQAtype::e_asSound] = 15;
        } else { // question was unset, switch off all answers
            m_level->answersAs[TQAtype::e_asSound].setValue(0);
            m_answersList[TQAtype::e_asSound] = 0;
        }
    }
    levelParamChanged();
    if (doEmit)
        emit updateLevel();
}

/** TODO: it is not used, @p setAnswers() is called directly from QML instead */
void TlevelCreatorItem::setAnswersAs(QList<int> aAs)
{
    for (int a = 0; a < 4; ++a) {
        m_level->answersAs[a].setValue(aAs[a]);
        switch (a) {
        case 0:
            m_level->questionAs.setOnScore(aAs[a] != 0);
            break;
        case 1:
            m_level->questionAs.setAsName(aAs[a] != 0);
            break;
        case 2:
            m_level->questionAs.setOnInstr(aAs[a] != 0);
            break;
        case 3:
            m_level->questionAs.setAsSound(aAs[a] != 0);
            break;
        }
    }
    levelParamChanged();
    emit updateLevel();
}

void TlevelCreatorItem::setAnswers(int questionType, int answersValue)
{
    if (questionType >= 0 && questionType < 4) {
        m_level->answersAs[questionType].setValue(answersValue);
        m_answersList[questionType] = answersValue;
        switch (questionType) {
        case 0:
            m_level->questionAs.setOnScore(answersValue != 0);
            break;
        case 1:
            m_level->questionAs.setAsName(answersValue != 0);
            break;
        case 2:
            m_level->questionAs.setOnInstr(answersValue != 0);
            break;
        case 3:
            m_level->questionAs.setAsSound(answersValue != 0);
            break;
        }
        levelParamChanged();
        emit updateLevel();
    }
}

bool TlevelCreatorItem::requireOctave() const
{
    return m_level->requireOctave;
}
void TlevelCreatorItem::setRequireOctave(bool require)
{
    m_level->requireOctave = require;
    levelParamChanged();
}

bool TlevelCreatorItem::requireStyle() const
{
    return m_level->requireStyle;
}
void TlevelCreatorItem::setRequireStyle(bool require)
{
    m_level->requireStyle = require;
    levelParamChanged();
}

bool TlevelCreatorItem::showStrNr() const
{
    return m_level->showStrNr;
}
void TlevelCreatorItem::setShowStrNr(bool showStr)
{
    m_level->showStrNr = showStr;
    levelParamChanged();
}

bool TlevelCreatorItem::onlyLowPos() const
{
    return m_level->onlyLowPos;
}
void TlevelCreatorItem::setOnlyLowPos(bool only)
{
    m_level->onlyLowPos = only;
    levelParamChanged();
}

bool TlevelCreatorItem::playMelody() const
{
    return m_level->canBeMelody() && m_level->questionAs.isOnScore() && m_level->answersAs[TQAtype::e_onScore].isSound();
}
void TlevelCreatorItem::setPlayMelody(bool play)
{
    m_level->answersAs[TQAtype::e_onScore].setAsSound(play);
    m_level->questionAs.setOnScore(m_level->answersAs[TQAtype::e_onScore].value() != 0);
    m_answersList[TQAtype::e_onScore] = m_level->answersAs[TQAtype::e_onScore].value();
    levelParamChanged();
    emit updateLevel();
}

bool TlevelCreatorItem::writeMelody() const
{
    return m_level->canBeMelody() && m_level->questionAs.isSound() && m_level->answersAs[TQAtype::e_asSound].isOnScore();
}
void TlevelCreatorItem::setWriteMelody(bool write)
{
    m_level->answersAs[TQAtype::e_asSound].setOnScore(write);
    m_level->questionAs.setAsSound(m_level->answersAs[TQAtype::e_asSound].value() != 0);
    m_answersList[TQAtype::e_asSound] = m_level->answersAs[TQAtype::e_asSound].value();
    levelParamChanged();
    emit updateLevel();
}

bool TlevelCreatorItem::repeatMelody() const
{
    return m_level->canBeMelody() && m_level->questionAs.isSound() && m_level->answersAs[TQAtype::e_asSound].isSound();
}
void TlevelCreatorItem::setRepeatMelody(bool repeat)
{
    m_level->answersAs[TQAtype::e_asSound].setAsSound(repeat);
    m_level->questionAs.setAsSound(m_level->answersAs[TQAtype::e_asSound].value() != 0);
    m_answersList[TQAtype::e_asSound] = m_level->answersAs[TQAtype::e_asSound].value();
    levelParamChanged();
    emit updateLevel();
}

bool TlevelCreatorItem::isMelody() const
{
    return m_level->melodyLen > 1;
}
void TlevelCreatorItem::setIsMelody(bool isMel)
{
    m_level->melodyLen = isMel ? 2 : 1;
    m_level->questionAs.setValue(0);
    for (int a = 0; a < 4; ++a) {
        m_level->answersAs[a].setValue(0);
        m_answersList[a] = 0;
    }
    levelParamChanged();
    emit updateLevel();
}

// Melodies page
int TlevelCreatorItem::melodyLen() const
{
    return m_level->melodyLen;
}
void TlevelCreatorItem::setMelodyLen(int len)
{
    if (m_level->melodyLen != len) {
        bool prevRtmState = m_level->useRhythms();
        m_level->melodyLen = len;
        levelParamChanged();
        if (m_level->useRhythms() != prevRtmState)
            emit hasRhythmsChanged();
    }
}

bool TlevelCreatorItem::endsOnTonic() const
{
    return m_level->endsOnTonic;
}
void TlevelCreatorItem::setEndsOnTonic(bool ends)
{
    m_level->endsOnTonic = ends;
    levelParamChanged();
}

/**
 * Due to @p Tlevel::EhowGetMelody enumerator has values 1, 2, 4 (power of 2) we need to convert them to ordered indexes: 0, 1, 2
 */
int TlevelCreatorItem::howGetMelody() const
{
    return qRound(qLn(static_cast<qreal>(m_level->howGetMelody) / qLn(2.0)));
}
void TlevelCreatorItem::setHowGetMelody(int hgm)
{
    auto rCast = static_cast<Tlevel::EhowGetMelody>(qPow(2.0, static_cast<qreal>(hgm)));
    if (rCast != m_level->howGetMelody) {
        m_level->howGetMelody = rCast;
        levelParamChanged();
        emit updateLevel();
    }
}

int TlevelCreatorItem::notesInList() const
{
    return m_level->notesList.count();
}
Tnote TlevelCreatorItem::noteFromList(int id) const
{
    return id >= 0 && id < m_level->notesList.count() ? m_level->notesList[id] : Tnote();
}

/**
 * Logic is simple, set note @p n if @p id matches range of the list
 * or simply append this note to the list end
 */
void TlevelCreatorItem::setNoteOfList(int id, const Tnote &n)
{
    if (id >= 0 && id < m_level->notesList.count())
        m_level->notesList[id] = n;
    else
        m_level->notesList << n;
}

int TlevelCreatorItem::keyOfRandList() const
{
    return static_cast<int>(m_level->keyOfrandList.value());
}
void TlevelCreatorItem::setKeyOfRandList(int key)
{
    m_level->keyOfrandList = TkeySignature(static_cast<char>(key));
    levelParamChanged();
}

bool TlevelCreatorItem::randomOrder() const
{
    return m_level->randOrderInSet;
}
void TlevelCreatorItem::setRandomOrder(bool randO)
{
    m_level->randOrderInSet = randO;
    levelParamChanged();
}

int TlevelCreatorItem::repeatsNumber() const
{
    return m_level->repeatNrInSet;
}
void TlevelCreatorItem::setRepeatsNumber(int rNr)
{
    m_level->repeatNrInSet = static_cast<quint8>(rNr);
    levelParamChanged();
}

// Range page
int TlevelCreatorItem::loFret() const
{
    return static_cast<int>(m_level->loFret);
}
void TlevelCreatorItem::setLoFret(int lf)
{
    if (static_cast<char>(lf) != m_level->loFret) {
        m_level->loFret = static_cast<char>(lf);
        levelParamChanged();
        emit updateLevel();
    }
}

int TlevelCreatorItem::hiFret() const
{
    return static_cast<int>(m_level->hiFret);
}
void TlevelCreatorItem::setHiFret(int hf)
{
    if (static_cast<char>(hf) != m_level->hiFret) {
        m_level->hiFret = static_cast<char>(hf);
        levelParamChanged();
        emit updateLevel();
    }
}

Tnote TlevelCreatorItem::loNote() const
{
    return m_level->loNote;
}
void TlevelCreatorItem::setLoNote(const Tnote &n)
{
    if (!m_level->loNote.compareNotes(n) && n.isValid()) {
        m_level->loNote = n;
        levelParamChanged();
    }
}

Tnote TlevelCreatorItem::hiNote() const
{
    return m_level->hiNote;
}
void TlevelCreatorItem::setHiNote(const Tnote &n)
{
    if (!m_level->hiNote.compareNotes(n) && n.isValid()) {
        m_level->hiNote = n;
        levelParamChanged();
    }
}

int TlevelCreatorItem::clef() const
{
    return static_cast<int>(m_level->clef.type());
}
void TlevelCreatorItem::setClef(int c)
{
    m_level->clef.setClef(static_cast<Tclef::EclefType>(c));
    levelParamChanged();
}

int TlevelCreatorItem::usedStrings() const
{
    return (m_level->usedStrings[0] ? 1 : 0) + (m_level->usedStrings[1] ? 2 : 0) + (m_level->usedStrings[2] ? 4 : 0) + (m_level->usedStrings[3] ? 8 : 0)
        + (m_level->usedStrings[4] ? 16 : 0) + (m_level->usedStrings[5] ? 32 : 0);
}

void TlevelCreatorItem::setUsedStrings(int uStr)
{
    for (int s = 0; s < 6; ++s)
        m_level->usedStrings[s] = (uStr & qRound(qPow(2.0, static_cast<qreal>(s)))) > 0;
    levelParamChanged();
}

void TlevelCreatorItem::adjustFretsToScale()
{
    char loF, hiF;
    if (m_level->adjustFretsToScale(loF, hiF)) {
        if (loF != m_level->loFret || hiF != m_level->hiFret) {
            m_level->loFret = loF;
            m_level->hiFret = hiF;
            levelParamChanged();
            emit updateLevel();
        }
    } else
        qDebug() << "[TlevelCreatorItem] Can't adjust fret range!";
}

void TlevelCreatorItem::adjustNotesToFretRange()
{
    Tnote loN(GLOB->loString().chromatic() + static_cast<short>(m_level->loFret));
    Tnote hiN(GLOB->hiString().chromatic() + static_cast<short>(m_level->hiFret));
    if (!loN.compareNotes(m_level->loNote) || !hiN.compareNotes(m_level->hiNote)) {
        m_level->loNote = loN;
        m_level->hiNote = hiN;
        levelParamChanged();
        emit updateLevel();
    }
}

// Accidentals page
bool TlevelCreatorItem::withSharps() const
{
    return m_level->withSharps;
}
void TlevelCreatorItem::setWithSharps(bool sharps)
{
    m_level->withSharps = sharps;
    levelParamChanged();
}

bool TlevelCreatorItem::withFlats() const
{
    return m_level->withFlats;
}
void TlevelCreatorItem::setWithFlats(bool flats)
{
    m_level->withFlats = flats;
    levelParamChanged();
}

bool TlevelCreatorItem::withDblAccids() const
{
    return m_level->withDblAcc;
}
void TlevelCreatorItem::setWithDblAccids(bool dblAccids)
{
    m_level->withDblAcc = dblAccids;
    levelParamChanged();
}

bool TlevelCreatorItem::forceAccids() const
{
    return m_level->forceAccids;
}
void TlevelCreatorItem::setForceAccids(bool fa)
{
    m_level->forceAccids = fa;
    levelParamChanged();
}

bool TlevelCreatorItem::useKeySign() const
{
    return m_level->useKeySign;
}
void TlevelCreatorItem::setUseKeySign(bool useKeys)
{
    m_level->useKeySign = useKeys;
    levelParamChanged();
}

bool TlevelCreatorItem::isSingleKey() const
{
    return m_level->isSingleKey;
}
void TlevelCreatorItem::setIsSingleKey(bool isSingle)
{
    m_level->isSingleKey = isSingle;
    levelParamChanged();
}

int TlevelCreatorItem::loKey() const
{
    return static_cast<int>(m_level->loKey.value());
}
void TlevelCreatorItem::setLoKey(int k)
{
    m_level->loKey = TkeySignature(static_cast<char>(k));
    if (k < 0)
        m_level->withFlats = true;
    if (k > 0)
        m_level->withSharps = true;
    levelParamChanged();
    emit updateLevel();
}

int TlevelCreatorItem::hiKey() const
{
    return static_cast<int>(m_level->hiKey.value());
}
void TlevelCreatorItem::setHiKey(int k)
{
    m_level->hiKey = TkeySignature(static_cast<char>(k));
    if (k < 0)
        m_level->withFlats = true;
    if (k > 0)
        m_level->withSharps = true;
    levelParamChanged();
    emit updateLevel();
}

bool TlevelCreatorItem::onlyCurrKey() const
{
    return m_level->onlyCurrKey;
}
void TlevelCreatorItem::setOnlyCurrKey(bool only)
{
    m_level->onlyCurrKey = only;
    levelParamChanged();
}

bool TlevelCreatorItem::manualKey() const
{
    return m_level->manualKey;
}
void TlevelCreatorItem::setManualKey(bool manual)
{
    m_level->manualKey = manual;
    levelParamChanged();
}

// Rhythms page
int TlevelCreatorItem::meters() const
{
    return static_cast<int>(m_level->meters);
}
void TlevelCreatorItem::setMeters(int m)
{
    bool prevRtmState = m_level->useRhythms();
    m_level->meters = static_cast<quint16>(m);
    levelParamChanged();
    if (m_level->useRhythms() != prevRtmState)
        emit hasRhythmsChanged();
}

quint32 TlevelCreatorItem::basicRhythms() const
{
    return m_level->basicRhythms;
}
void TlevelCreatorItem::setBasicRhythms(quint32 br)
{
    bool prevRtmState = m_level->useRhythms();
    m_level->basicRhythms = br;
    levelParamChanged();
    if (m_level->useRhythms() != prevRtmState)
        emit hasRhythmsChanged();
}

quint32 TlevelCreatorItem::dotsRhythms() const
{
    return m_level->dotsRhythms;
}
void TlevelCreatorItem::setDotsRhythms(quint32 dr)
{
    bool prevRtmState = m_level->useRhythms();
    m_level->dotsRhythms = dr;
    levelParamChanged();
    if (m_level->useRhythms() != prevRtmState)
        emit hasRhythmsChanged();
}

int TlevelCreatorItem::rhythmDiversity() const
{
    return m_level->rhythmDiversity;
}
void TlevelCreatorItem::setRhythmDiversity(int diversity)
{
    m_level->rhythmDiversity = diversity;
    levelParamChanged();
}

int TlevelCreatorItem::barNumber() const
{
    return m_level->barNumber;
}
void TlevelCreatorItem::setBarNumber(int bNr)
{
    m_level->barNumber = bNr;
    levelParamChanged();
}

bool TlevelCreatorItem::variableBarNr() const
{
    return m_level->variableBarNr;
}
void TlevelCreatorItem::setVariableBarNr(bool variabus)
{
    m_level->variableBarNr = variabus;
    levelParamChanged();
}

bool TlevelCreatorItem::useRests() const
{
    return m_level->useRests;
}
void TlevelCreatorItem::setUseRests(bool rests)
{
    m_level->useRests = rests;
    levelParamChanged();
}

bool TlevelCreatorItem::useTies() const
{
    return m_level->useTies;
}
void TlevelCreatorItem::setUseTies(bool ties)
{
    m_level->useTies = ties;
    levelParamChanged();
}

bool TlevelCreatorItem::hasRhythms() const
{
    return m_level->useRhythms();
}

void TlevelCreatorItem::openLevel(const QString &levelFile)
{
    if (selector()) {
        selector()->loadFromFile(levelFile);
        *m_level = *m_selector->currentLevel();
    } else { // delay is necessary because selector is created when level creator component is completed
        QTimer::singleShot(200, this, [=] {
            if (selector()) {
                selector()->loadFromFile(levelFile);
                *m_level = *m_selector->currentLevel();
            } else
                qDebug() << "[TlevelCreatorItem] device is too slow to open file as command line argument.";
        });
    }
}

void TlevelCreatorItem::melodyListChanged()
{
    levelParamChanged();
}

void TlevelCreatorItem::resumeAfterLevelChange()
{
    if (!m_resumeAfterLevelChange)
        return;

    m_resumeAfterLevelChange = false;
    if (!m_titleExtension.isEmpty()) {
        m_titleExtension.clear();
        emit saveStateChanged();
    }
    *m_level = *m_selector->currentLevel();
    m_answersList.clear();
    m_answersList << m_level->answersAs[0].value() << m_level->answersAs[1].value() << m_level->answersAs[2].value() << m_level->answersAs[3].value();
    emit updateLevel();
    if (m_level->howGetMelody == Tlevel::e_randFromList)
        emit updateNotesList();
    else if (m_level->howGetMelody == Tlevel::e_melodyFromSet)
        emit updateMelodyList();
}

// #################################################################################################
// ###################              PROTECTED           ############################################
// #################################################################################################

void TlevelCreatorItem::whenLevelChanged()
{
    if (m_selector->currentLevel() == nullptr) {
        qDebug() << "[TlevelCreatorItem] Null pointer of level is selected!";
        return;
    }

    m_resumeAfterLevelChange = true;
    if (!m_titleExtension.isEmpty())
        emit wantNotSavedMessage(tr("level not saved!").toUpper(), tr("Level was changed and not saved!"));
    else
        resumeAfterLevelChange();
}

QStringList TlevelCreatorItem::validateLevel()
{
    QStringList res;
    // Check has a level sense - are there an questions and answers
    if (!m_level->canBeScore() && !m_level->canBeName() && !m_level->canBeInstr() && !m_level->canBeSound()) {
        res << tr("There aren't any questions or answers selected.<br>Level makes no sense.");
        return res;
    }
    // checking range
    // determine the highest note of fret range on available strings
    if (m_level->canBeInstr() || (m_level->instrument != Tinstrument::NoInstrument && m_level->answerIsSound())) {
        // only when guitar is enabled otherwise frets range was adjusted automatically
        int hiAvailStr, loAvailStr, cnt = -1;
        do {
            cnt++;
        } while (!m_level->usedStrings[GLOB->strOrder(cnt)] && cnt < GLOB->Gtune()->stringNr());
        hiAvailStr = GLOB->strOrder(cnt);
        cnt = GLOB->Gtune()->stringNr();
        do {
            cnt--;
        } while (!m_level->usedStrings[GLOB->strOrder(cnt)] && cnt >= 0);
        loAvailStr = GLOB->strOrder(cnt);
        if (m_level->loNote.chromatic() > GLOB->Gtune()->str(hiAvailStr + 1).chromatic() + m_level->hiFret
            || m_level->hiNote.chromatic() < GLOB->Gtune()->str(loAvailStr + 1).chromatic() + m_level->loFret)
            res << tr("<li>Range of frets is beyond the scale of this level</li>");
    }
    // Check is level range fit to instrument scale
    if (m_level->canBeInstr() || m_level->answerIsSound()) {
        if (!m_level->inScaleOf(GLOB->loString().chromatic(), GLOB->hiString().chromatic() + GLOB->GfretsNumber))
            res << "<li>" + TlevelSelector::rangeBeyondScaleTxt() + "</li>";
    }
    // checking are accidentals needed because of hi and low notes in range
    char acc = 0;
    if (m_level->loNote.alter())
        acc = m_level->loNote.alter();
    if (m_level->hiNote.alter())
        acc = m_level->hiNote.alter();
    if (acc) {
        if ((acc == 1 && !m_level->withSharps) || (acc == -1 && !m_level->withFlats))
            res << tr("<li>In range of notes some accidentals are used<br>but not available in this level</li>");
    }
    // Force accidentals enabled but any accidental was selected
    if (m_level->forceAccids && (!m_level->withFlats && !m_level->withSharps && !m_level->withDblAcc))
        res << tr("<li>Force appropriate accidental is enabled but any accidental was selected.</li>");
    // When no accidentals and no style and question and answers are note name
    if ((m_level->questionAs.isName() && m_level->answersAs[TQAtype::e_asName].isName())) {
        if ((m_level->withFlats && m_level->withSharps) || m_level->withDblAcc || m_level->requireStyle) {
            // do nothing - level is valid
        } else
            res << tr(
                "<li>Questions and answers as note names will be the same. To avoid that level has to use flats and sharps and/or double accidentals and/or to "
                "use different name styles.</li>");
    }
    // Check is possible of using naming style
    if (m_level->requireStyle && !m_level->canBeName())
        res << tr(
            "<li>'Use different naming styles' was checked but neither questions nor answers as note name are checked.<br>Check this type of answer/question "
            "or uncheck 'Use different naming styles'.</li>");
    // Check are questions and answers as note on the staff have sense (are different)
    if (m_level->questionAs.isOnScore() && m_level->answersAs[TQAtype::e_onScore].isOnScore())
        if (!m_level->manualKey && !m_level->forceAccids)
            res << tr(
                "<li>Questions and answers as notes on the staff will be the same. Manually selecting keys or forcing accidentals has to be selected to avoid "
                "that.</li>");
    // Check is possible of manualKey
    if (m_level->useKeySign && m_level->manualKey) {
        if (!m_level->answersAs[TQAtype::e_onScore].isOnScore() && !m_level->answersAs[TQAtype::e_asName].isOnScore()
            && !m_level->answersAs[TQAtype::e_onInstr].isOnScore() && !m_level->answersAs[TQAtype::e_asSound].isOnScore())
            res << tr("<li>Manual selecting of a key signature was checked but answer as note on the staff was not checked.</li>");
    }
    // 'Fret to fret' has to have suitable fret range to be possible
    if (m_level->questionAs.isOnInstr() && m_level->answersAs[TQAtype::e_onInstr].isOnInstr()) {
        int minRange = 0; // first determine a minimal range for current tune
        int startStr = GLOB->Gtune()->str(GLOB->strOrder(0) + 1).chromatic();
        for (int i = 1; i < GLOB->Gtune()->stringNr(); i++) {
            minRange = qMax(minRange, startStr - GLOB->Gtune()->str(GLOB->strOrder(i) + 1).chromatic());
            startStr = GLOB->Gtune()->str(GLOB->strOrder(i) + 1).chromatic();
        }
        if (m_level->hiFret - m_level->loFret < minRange)
            res << tr("<li>Fret range is not enough to find any note in different positions. At least <b>%1</b> frets range is required.</li>").arg(minRange);
    }
    // Melodies finished on tonic have to contain the tonic note in range
    if (m_level->canBeMelody() && m_level->endsOnTonic) {
        bool inKeyNotes[7]; // array with notes used by key signatures from range
        for (int i = 0; i < 7; ++i)
            inKeyNotes[i] = false;
        for (int i = m_level->loKey.value(); i <= m_level->hiKey.value(); ++i) {
            inKeyNotes[static_cast<int>(TkeySignature::minorKeys[i + 7])] = true;
            inKeyNotes[static_cast<int>(TkeySignature::majorKeys[i + 7])] = true;
        }
        int startNote = m_level->loNote.note() + (m_level->loNote.octave() + 5) * 7 - 1;
        int endNote = m_level->hiNote.note() + (m_level->hiNote.octave() + 5) * 7 - 1;
        for (int n = 0; n < 7; ++n) {
            if (inKeyNotes[n]) {
                bool found = false;
                for (int i = startNote; i <= endNote; ++i) {
                    if (n == i % 7) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    res << tr("<li>Possible missing a tonic note for some key signatures in selected note range .</li>");
                    break;
                }
            }
        }
    }
    // Note list is empty
    if (m_level->canBeMelody() && m_level->howGetMelody == Tlevel::e_randFromList) {
        if (m_level->notesList.count() < 2)
            res << tr("<li>There are not enough selected notes to create a melody.</li>");
    }
    // When level is set of melodies but no melody was added
    if (m_level->canBeMelody() && m_level->howGetMelody == Tlevel::e_melodyFromSet) {
        if (m_level->melodySet.isEmpty())
            res << tr("<li>No melody was added to the list.</li>");
    }

    return res;
}

void TlevelCreatorItem::showValidationMessage(QStringList &message)
{
    if (!message.isEmpty()) {
        QString title = tr("Level validation");
        if (message.last().contains(QLatin1String("</li>"))) { // when <li> exist - show warning
            message.replaceInStrings(QLatin1String("</li>"), QString());
            message.replaceInStrings(QLatin1String("<li>"), QString());
            emit wantValidationMessage(tr("Seems like this level has some mistakes") + QLatin1String(":"), message, Qt::yellow);
        } else // no questions nor answers
            emit wantValidationMessage(title, message, Qt::red);
    }
}

// #################################################################################################
// #####################              PRIVATE           ############################################
// #################################################################################################

void TlevelCreatorItem::levelParamChanged()
{
    if (m_titleExtension.isEmpty()) {
        m_titleExtension = QLatin1String("  (") + tr("level not saved!") + QLatin1String(")");
        emit saveStateChanged();
    }
    m_selector->levelPreview()->setLevel(m_level);
}
