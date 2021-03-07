/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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


#ifndef TGOTIT_H
#define TGOTIT_H

#include <QtCore/qobject.h>


class TscoreObject;
class TnoteStruct;
class Tnote;
class QTimer;


/**
 * This is C++ logic/proxy to handle 'Got It!' mechanism.
 * NOTICE: Every instance of this class on QML side
 * has to set @p gotItType apparently to use helpers according to 'Got It' type.
 */
class TgotIt : public QObject
{

  Q_OBJECT

  Q_PROPERTY(EgotItType gotItType READ gotItType WRITE setGotItType NOTIFY gotItTypeChanged)
  Q_PROPERTY(TscoreObject* score READ score WRITE setScore)
  Q_PROPERTY(int maxVolume READ maxVolume NOTIFY maxVolumeChanged)
  Q_PROPERTY(QString noteName READ noteName NOTIFY noteNameChanged)

public:
  explicit TgotIt(QObject* parent = nullptr);
    ~TgotIt() override;

  enum EgotItType {
    GotAnything = 0,
    GotSoundInfo,
    GotExamOrExer,
    GotExamFlow,
    GotHandleScore,
    GotNoteSelected
  };
  Q_ENUM(EgotItType)

  EgotItType gotItType() const { return m_gotItType; }
  void setGotItType(EgotItType gt);

// GotSoundInfo ======================================
  int maxVolume() const { return static_cast<int>(m_maxVolume * 100.0); }
  QString noteName() const { return m_noteName; }

// GotHandleScore ======================================
  TscoreObject* score() { return m_score; }
  void setScore(TscoreObject* sc);
      /**
       * Selects accidental.
       * Available only for @p GotHandleScore.
       */
  Q_INVOKABLE void setCursorAlter(int curAlt);

      /**
       * Selects rhythmic value.
       * Available only for @p GotHandleScore.
       */
  Q_INVOKABLE void setWorkRtmValue(int rtmV);

// GotExamOrExer =======================================
      /**
       * Help text (HTML) about what is exercise and exam
       */
  Q_INVOKABLE static QString exerOrExamHelpTxt();

// GotExamFlow =========================================
  Q_INVOKABLE QString getQuestionText() const;
  Q_INVOKABLE QString confirmText() const;
  Q_INVOKABLE QString practiceText() const;
  Q_INVOKABLE QString practiceExplained() const;
  Q_INVOKABLE QString examText() const;
  Q_INVOKABLE QString examExplained() const;
  Q_INVOKABLE QString optionsText() const;
  Q_INVOKABLE QString onlineDocP(const QString& hash) const;

// GotNoteSelected
  Q_INVOKABLE QString noteCursorText() const;
  Q_INVOKABLE QString ifSelectedText() const;
  Q_INVOKABLE QString ifNotSelectedText() const;
  Q_INVOKABLE QString unselectText() const;

signals:
  void gotItTypeChanged();
  void maxVolumeChanged();
  void noteNameChanged();


protected:
  void noteStartedSlot(const Tnote& n);
  void noteFinishedSlot(TnoteStruct);


private:
  EgotItType                     m_gotItType = GotAnything;

// GotHandleScore ====================================
  TscoreObject                  *m_score = nullptr;

// GotSoundInfo ======================================
  qreal                          m_maxVolume = 0.0;
  QTimer                        *m_soundTimer;
  QString                        m_noteName;
};

#endif // TGOTIT_H
