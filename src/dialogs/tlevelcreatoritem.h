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

#ifndef TLEVELCREATORITEM_H
#define TLEVELCREATORITEM_H


#include <music/tnote.h>
#include <QtQuick/qquickitem.h>


class Tlevel;
class TlevelSelector;


/**
 * This class is QML logic for "Level creator" dialog window
 */
class TlevelCreatorItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(TlevelSelector* selector READ selector WRITE setSelector)
  Q_PROPERTY(QString title READ title NOTIFY saveStateChanged)
  Q_PROPERTY(bool notSaved READ notSaved NOTIFY saveStateChanged)
  // Questions
  Q_PROPERTY(int questionAs READ questionAs WRITE setQuestionAs NOTIFY updateLevel)
  Q_PROPERTY(QList<int> answersAs READ answersAs WRITE setAnswersAs NOTIFY updateLevel)
  Q_PROPERTY(bool requireOctave READ requireOctave WRITE setRequireOctave NOTIFY updateLevel)
  Q_PROPERTY(bool requireStyle READ requireStyle WRITE setRequireStyle NOTIFY updateLevel)
  Q_PROPERTY(bool showStrNr READ showStrNr WRITE setShowStrNr NOTIFY updateLevel)
  Q_PROPERTY(bool onlyLowPos READ onlyLowPos WRITE setOnlyLowPos NOTIFY updateLevel)
  Q_PROPERTY(bool isMelody READ isMelody WRITE setIsMelody NOTIFY updateLevel)
  Q_PROPERTY(qreal playMelody READ playMelody WRITE setPlayMelody NOTIFY updateLevel)
  Q_PROPERTY(qreal writeMelody READ writeMelody WRITE setWriteMelody NOTIFY updateLevel)
  Q_PROPERTY(qreal repeatMelody READ repeatMelody WRITE setRepeatMelody NOTIFY updateLevel)
  // Melodies
  Q_PROPERTY(int melodyLen READ melodyLen WRITE setMelodyLen NOTIFY updateLevel)
  // Range
  Q_PROPERTY(int loFret READ loFret WRITE setLoFret NOTIFY updateLevel)
  Q_PROPERTY(int hiFret READ hiFret WRITE setHiFret NOTIFY updateLevel)
  Q_PROPERTY(Tnote loNote READ loNote WRITE setLoNote NOTIFY updateLevel)
  Q_PROPERTY(Tnote hiNote READ hiNote WRITE setHiNote NOTIFY updateLevel)
  Q_PROPERTY(int clef READ clef WRITE setClef NOTIFY updateLevel)
  Q_PROPERTY(int usedStrings READ usedStrings WRITE setUsedStrings NOTIFY updateLevel)
  // Accidentals
  Q_PROPERTY(bool withSharps READ withSharps WRITE setWithSharps NOTIFY updateLevel)
  Q_PROPERTY(bool withFlats READ withFlats WRITE setWithFlats NOTIFY updateLevel)
  Q_PROPERTY(bool withDblAccids READ withDblAccids WRITE setWithDblAccids NOTIFY updateLevel)
  Q_PROPERTY(bool forceAccids READ forceAccids WRITE setForceAccids NOTIFY updateLevel)
  Q_PROPERTY(bool useKeySign READ useKeySign WRITE setUseKeySign NOTIFY updateLevel)
  Q_PROPERTY(bool isSingleKey READ isSingleKey WRITE setIsSingleKey NOTIFY updateLevel)
  Q_PROPERTY(int loKey READ loKey WRITE setLoKey NOTIFY updateLevel)
  Q_PROPERTY(int hiKey READ hiKey WRITE setHiKey NOTIFY updateLevel)
  Q_PROPERTY(bool onlyCurrKey READ onlyCurrKey WRITE setOnlyCurrKey NOTIFY updateLevel)
  Q_PROPERTY(bool manualKey READ manualKey WRITE setManualKey NOTIFY updateLevel)

public:
  TlevelCreatorItem(QQuickItem* parent = nullptr);
  ~TlevelCreatorItem() override;

  TlevelSelector* selector() { return m_selector; }
  void setSelector(TlevelSelector* sel);

      /**
       * Text about unsaved level that extends creator window title on title bar
       */
  QString title() const;

  bool notSaved() const;

  // Questions page
  int questionAs() const;
  void setQuestionAs(int qAs);

  QList<int> answersAs() const { return m_answersList; }
  void setAnswersAs(QList<int> aAs);

  Q_INVOKABLE void setAnswers(int questionType, int answersValue);

  bool requireOctave() const;
  void setRequireOctave(bool require);

  bool requireStyle() const;
  void setRequireStyle(bool require);

  bool showStrNr() const;
  void setShowStrNr(bool showStr);

  bool onlyLowPos() const;
  void setOnlyLowPos(bool only);

  bool playMelody() const;
  void setPlayMelody(bool play);

  bool writeMelody() const;
  void setWriteMelody(bool write);

  bool repeatMelody() const;
  void setRepeatMelody(bool repeat);

  bool isMelody() const;
  void setIsMelody(bool isMel);

  // Melodies page
  int melodyLen() const;
  void setMelodyLen(int len);

  // Range page
  int loFret() const;
  void setLoFret(int lf);

  int hiFret() const;
  void setHiFret(int hf);

  Tnote loNote() const;
  void setLoNote(const Tnote& n);

  Tnote hiNote() const;
  void setHiNote(const Tnote& n);

  int clef() const;
  void setClef(int c);

      /**
       * Returned number represents bits of every enabled string
       */
  int usedStrings() const;
  void setUsedStrings(int uStr);

  // Accidentals page
  bool withSharps() const;
  void setWithSharps(bool sharps);

  bool withFlats() const;
  void setWithFlats(bool flats);

  bool withDblAccids() const;
  void setWithDblAccids(bool dblAccids);

  bool forceAccids() const;
  void setForceAccids(bool fa);

  bool useKeySign() const;
  void setUseKeySign(bool useKeys);

  bool isSingleKey() const;
  void setIsSingleKey(bool isSingle);

  int loKey() const;
  void setLoKey(int k);

  int hiKey() const;
  void setHiKey(int k);

  bool onlyCurrKey() const;
  void setOnlyCurrKey(bool only);

  bool manualKey() const;
  void setManualKey(bool manual);

signals:
  void updateLevel();
  void saveStateChanged();

protected:
  void whenLevelChanged();

private:
      /**
       * Common routines when any level parameter is changed by user
       */
  void levelParamChanged();

private:
  Tlevel                    *m_level = nullptr;
  TlevelSelector            *m_selector = nullptr;
  QString                    m_title, m_titleExtension;
  QList<int>                 m_answersList; /**< Each number represents bitwised value of @p TQAtype  */
};

#endif // TLEVELCREATORITEM_H
