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
class Ttechnical;


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
  Q_PROPERTY(Taction* playAct READ playAct)
  Q_PROPERTY(Taction* recModeAct READ recModeAct)
  Q_PROPERTY(Taction* showNamesAct READ showNamesAct)
  Q_PROPERTY(Taction* extraAccidsAct READ extraAccidsAct)
  Q_PROPERTY(Taction* deleteLastAct READ deleteLastAct)
  Q_PROPERTY(Taction* clearScoreAct READ clearScoreAct)
  Q_PROPERTY(Taction* openXmlAct READ openXmlAct)
  Q_PROPERTY(Taction* saveXmlAct READ saveXmlAct)
  Q_PROPERTY(Taction* zoomOutAct READ zoomOutAct)
  Q_PROPERTY(Taction* zoomInAct READ zoomInAct)
  Q_PROPERTY(QList<QObject*> scoreActions READ scoreActions NOTIFY scoreActionsChanged)
  Q_PROPERTY(QString keyNameText READ keyNameText NOTIFY keyNameTextChanged)

public:
  explicit TmainScoreObject(QObject* parent = nullptr);
  ~TmainScoreObject() override;

  static TmainScoreObject* instance() { return m_instance; }

  TscoreObject* scoreObject() { return m_scoreObj; }
  void setScoreObject(TscoreObject* scoreObj);

  Taction* playAct() { return m_playAct; }
  Taction* recModeAct() { return m_recModeAct; }
  Taction* showNamesAct() { return m_showNamesAct; }
  Taction* extraAccidsAct() { return m_extraAccidsAct; }
  Taction* deleteLastAct() { return m_deleteLastAct; }
  Taction* clearScoreAct() { return m_clearScoreAct; }
  Taction* openXmlAct() { return m_openXmlAct; }
  Taction* saveXmlAct() { return m_saveXmlAct; }
  Taction* zoomOutAct() { return m_zoomOutAct; }
  Taction* zoomInAct() { return m_zoomInAct; }

  QList<QObject*> scoreActions() { return m_scoreActions; }

  QString keyNameText() const;

// redirected from TscoreObject
  void setReadOnly(bool ro);
  void clearScore();
  void setKeySignature(const TkeySignature& key);
  char keySignatureValue();
  Tnote getNote(int id);
  void setSelectedItem(int id);
  void setTechnical(int noteId, quint32 tech);
  bool selectInReadOnly() const;
  void setSelectInReadOnly(bool sel);
  quint32 technical(int noteId);

// exam/exercise related
  void askQuestion(const Tnote& note, quint32 technicalData = 255);
  void askQuestion(const Tnote& note, const TkeySignature& key, quint32 technicalData = 255);
  void askQuestion(Tmelody* mel);
  void prepareKeyToAnswer(const TkeySignature& fakeKey, const QString& expectKeyName);

      /**
       * Sets given accidental
       */
  void forceAccidental(int accid);
  void unLockScore();
  void lockKeySignature(bool lock);
  void markNoteHead(const QColor& outColor, int noteNr);

signals:
  void scoreActionsChanged();
  void keyNameTextChanged();
  // redirected from TscoreObject
  void clicked();
  void readOnlyNoteClicked(int noteId);

protected:
  void openXmlActSlot();
  void saveXmlActSlot();
  void isExamChangedSlot();
  void singleModeSlot();
  void paletteSlot();

  QColor scoreBackgroundColor(const QColor& c, int alpha);
  void checkSingleNoteVisibility();

private:
  TscoreObject              *m_scoreObj = nullptr;
  Taction                   *m_playAct, *m_recModeAct;
  Taction                   *m_showNamesAct, *m_extraAccidsAct;
  Taction                   *m_zoomOutAct, *m_zoomInAct;
  Taction                   *m_deleteLastAct, *m_clearScoreAct;
  Taction                   *m_openXmlAct, *m_saveXmlAct;
  QList<QObject*>            m_scoreActions;

  QQuickItem                *m_questionMark = nullptr;
  QQuickItem                *m_questionKey = nullptr;

  static TmainScoreObject   *m_instance;

};

#endif // TMAINSCOREOBJECT_H
