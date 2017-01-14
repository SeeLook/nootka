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

#ifndef DENOTE_H
#define DENOTE_H


#include <QtQuick/qquickitem.h>


class TscoreNote;
class Tnote;


class DeNote : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(int pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
  Q_PROPERTY(int octave READ octave WRITE setOctave NOTIFY octaveChanged)
  Q_PROPERTY(int alter READ alter WRITE setAlter NOTIFY alterChanged)
  Q_PROPERTY(Erhythm rhythm READ rhythm WRITE setRhythm NOTIFY rhythmChanged)

  friend class DeScore;

public:
  DeNote(QQuickItem* parent = nullptr);
  virtual ~DeNote();

  char pitch();
  void setPitch(char p);

  char octave();
  void setOctave(char o);

  char alter();
  void setAlter(char a);

      /**
       * This enum maps @p Trhythm::Eryhythm enum to expose it to QML
       * It is easier than registering original and changing 
       * @p e_something to proper format: @p Something
       */
  enum Erhythm : quint8 {
    NoRhythm = 0, Whole = 1, Half = 2, Quarter = 3, Eighth = 4, Sixteenth = 5
  };
  Q_ENUM(Erhythm)

  Erhythm rhythm();
  void setRhythm(Erhythm r);

signals:
  void pitchChanged();
  void octaveChanged();
  void alterChanged();
  void rhythmChanged();


protected:
  void setNoteSegment(TscoreNote* ns);
  TscoreNote* noteSegment() { return m_scoreNote; }
  Tnote* note() { return m_note; }

private:
  TscoreNote              *m_scoreNote;
  Tnote                   *m_note;
};

#endif // DENOTE_H
