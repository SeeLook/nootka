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

#ifndef TMAINSCOREOBJECT_H
#define TMAINSCOREOBJECT_H


#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>


class Taction;
class TscoreObject;
class Tnote;
class TkeySignature;
class Tmelody;
class QQuickItem;
class QQmlComponent;
class Ttechnical;
class TstaffLines;


#define MAIN_SCORE TmainScoreObject::instance()


/**
 * @p TmainScoreObject is yet another helper to divide score implementation and move its logic to C++
 * It manages all score actions, but their shortcuts have to be set on QML side.
 * @p scoreActions() is the list of currently available actions used by @p ScoreMenuContent.qml as a model.
 * The list changes during exam.
 * IT HAS TO HAVE @p TscoreObject set through @p scoreObject QML property
 */
class TmainScoreObject : public QObject
{

  Q_OBJECT

  Q_PROPERTY(TscoreObject* scoreObject READ scoreObject WRITE setScoreObject)
  Q_PROPERTY(Taction* showNamesAct READ showNamesAct)
  Q_PROPERTY(Taction* extraAccidsAct READ extraAccidsAct)
  Q_PROPERTY(Taction* zoomOutAct READ zoomOutAct)
  Q_PROPERTY(Taction* zoomInAct READ zoomInAct)
  Q_PROPERTY(QList<QObject*> scoreActions READ scoreActions NOTIFY scoreActionsChanged)
  Q_PROPERTY(QList<QObject*> noteActions READ noteActions NOTIFY scoreActionsChanged)
  Q_PROPERTY(Taction* nextNoteAct READ prevNoteAct)
  Q_PROPERTY(Taction* playAct READ playAct)
  Q_PROPERTY(Taction* recModeAct READ recModeAct)
  Q_PROPERTY(Taction* openXmlAct READ openXmlAct)
  Q_PROPERTY(Taction* saveXmlAct READ saveXmlAct)
  Q_PROPERTY(Taction* randMelodyAct READ randMelodyAct)
  Q_PROPERTY(Taction* notesMenuAct READ notesMenuAct)
  Q_PROPERTY(QList<QObject*> melodyActions READ melodyActions NOTIFY melodyActionsChanged)
  Q_PROPERTY(QString keyNameText READ keyNameText NOTIFY keyNameTextChanged)
  Q_PROPERTY(QQuickItem* mainScoreItem READ mainScoreItem WRITE setMainScoreItem)

public:
  explicit TmainScoreObject(QObject* parent = nullptr);
  ~TmainScoreObject() override;

  static TmainScoreObject* instance() { return m_instance; }

  TscoreObject* scoreObject() { return m_scoreObj; }
  void setScoreObject(TscoreObject* scoreObj);

  Taction* showNamesAct() { return m_showNamesAct; }
  Taction* extraAccidsAct() { return m_extraAccidsAct; }
  Taction* zoomOutAct() { return m_zoomOutAct; }
  Taction* zoomInAct() { return m_zoomInAct; }

  QList<QObject*> scoreActions() { return m_scoreActions; }
  QList<QObject*> noteActions() { return m_noteActions; }

  Taction* nextNoteAct() { return m_nextNoteAct; }
  Taction* prevNoteAct() { return m_prevNoteAct; }

  Taction* playAct() { return m_playAct; }
  Taction* recModeAct() { return m_recModeAct; }
  Taction* openXmlAct() { return m_openXmlAct; }
  Taction* saveXmlAct() { return m_saveXmlAct; }
  Taction* randMelodyAct() { return m_randMelodyAct; }
  Taction* notesMenuAct() { return m_notesMenuAct; }

  QList<QObject*> melodyActions() { return m_melodyActions; }

  QString keyNameText() const;

  QQuickItem* mainScoreItem() { return m_mainScoreItem; }
  void setMainScoreItem(QQuickItem* msItem);

// redirected from TscoreObject
  int notesCount() const;
  void setReadOnly(bool ro);
  void clearScore();
  void setKeySignatureEnabled(bool enbleKey);
  void setKeySignature(const TkeySignature& key);
  char keySignatureValue();
  int clefType() const;
  void setClef(int clefType);
  int meter() const;
  void setMeter(int meterType);
  Tnote getNote(int id);
  int setSelectedItem(int id);
  void setTechnical(int noteId, quint32 tech);
  bool selectInReadOnly() const;
  void setSelectInReadOnly(bool sel);
  quint32 technical(int noteId);
  void setMelody(Tmelody* mel);
  void getMelody(Tmelody* melody);

  Taction* clearScoreAct();
  Taction* scoreMenuAct();

// exam/exercise related
  void askQuestion(const Tnote& note, quint32 technicalData = 255);
  void askQuestion(const Tnote& note, const TkeySignature& key, quint32 technicalData = 255);
  void askQuestion(Tmelody* mel, bool ignoreTechnical, const TkeySignature& melodyKey);
  void prepareKeyToAnswer(const TkeySignature& fakeKey, const QString& expectKeyName);
  void showNoteNames(bool showName);
  void showNoteName(int noteNr, bool showName);

      /**
       * Sets given accidental
       */
  void forceAccidental(int accid);
  void unLockScore();
  void lockKeySignature(bool lock);

      /**
       * Marks note head of index @p noteNr
       * If note starts a tie - all tied notes are marked as well.
       * Also when note is a rest - all continuous rests are treated as one
       * Returns how many notes was marked
       */
  int markNoteHead(const QColor& outColor, int noteNr);

  void correctNote(const Tnote& goodNote, bool corrAccid = false);
  void correctKeySignature(const TkeySignature& keySign);

      /**
       * @p TRUE if correction animation was started.
       * In this moment @p m_correctNoteId stores note item id,
       * otherwise it is -1
       */
  bool isCorrectAnimPending() { return m_correctNoteId > -1; }

  Q_INVOKABLE void saveMusicXml(const QString& fileName, const QString& title = QString(), const QString& composer = QString());
  Q_INVOKABLE QStringList recentTitles() const;
  Q_INVOKABLE QStringList recentComposers() const;

signals:
  void scoreActionsChanged();
  void melodyActionsChanged();
  void keyNameTextChanged();
  void correctionFinished();
  // redirected from TscoreObject
  void clicked();
  void readOnlyNoteClicked(int noteId);
  void melodyGenerate();
  void melodyNameDialog();

protected:
  void openXmlActSlot();
  void randMelodySlot();
  void isExamChangedSlot();
  void singleModeSlot();
  void paletteSlot();
  void getMelodyNameSlot();
  void playScoreSlot();

  QColor scoreBackgroundColor(const QColor& c, int alpha);
  void checkSingleNoteVisibility();

  void checkExtraStaves();

  protected slots:
    void applyCorrectSlot();
    void correctionFinishedSlot();

private:
  TscoreObject              *m_scoreObj = nullptr;
  Taction                   *m_playAct, *m_recModeAct;
  Taction                   *m_showNamesAct, *m_extraAccidsAct;
  Taction                   *m_zoomOutAct, *m_zoomInAct;
  Taction                   *m_openXmlAct, *m_saveXmlAct, *m_randMelodyAct;
  Taction                   *m_nextNoteAct, *m_prevNoteAct;
  Taction                   *m_notesMenuAct = nullptr;
  QList<QObject*>            m_scoreActions;
  QList<QObject*>            m_noteActions;
  QList<QObject*>            m_melodyActions;

  QQuickItem                *m_questionMark = nullptr;
  QQuickItem                *m_questionKey = nullptr;

  static TmainScoreObject   *m_instance;

  QQuickItem                *m_mainScoreItem = nullptr;

  QList<TstaffLines*>        m_emptyStaves;
  Tnote                     *m_goodNote;
  QObject                   *m_animationObj = nullptr;
  int                        m_correctNoteId = -1; /**< keeps id of note with correction anim, -1 no note, no anim */

};

#endif // TMAINSCOREOBJECT_H
