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

#ifndef TNAMEOBJECT_H
#define TNAMEOBJECT_H


#include <music/tnote.h>
#include <QtQuick/qquickitem.h>


class TnameItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(Tnote note READ note WRITE setNote NOTIFY noteChanged)
  Q_PROPERTY(int step READ step WRITE setStep NOTIFY stepChanged)
  Q_PROPERTY(int alter READ alter WRITE setAlter NOTIFY alterChanged)
  Q_PROPERTY(int octave READ octave WRITE setOctave NOTIFY octaveChanged)
  Q_PROPERTY(QString nameText READ nameText NOTIFY nameTextChanged)
  Q_PROPERTY(int nameStyle READ nameStyle WRITE setNameStyle NOTIFY nameStyleChanged)


public:
  TnameItem (QQuickItem* parent = nullptr);
  ~TnameItem() override;

  Tnote note() const { return m_note; }
  void setNote(const Tnote& n);

  int step() const { return static_cast<int>(m_note.note); }
  void setStep (int st);

  int alter() const { return static_cast<int>(m_note.alter); }
  void setAlter(int alt);

  int octave() const { return static_cast<int>(m_note.octave); }
  void setOctave(int oct);

  int nameStyle() const { return static_cast<int>(m_nameStyle); }
  void setNameStyle(int nStyle);

  QString nameText() const;

  Q_INVOKABLE QString octaveName(int oNr) const;
  Q_INVOKABLE QString octavesLink() const;
  Q_INVOKABLE QString noteButtonText(int noteNr, int nStyle = -1);

signals:
  void noteChanged();
  void stepChanged();
  void alterChanged();
  void octaveChanged();
  void nameTextChanged();
  void nameStyleChanged();

private:
  Tnote                   m_note;
  Tnote::EnameStyle       m_nameStyle;
};

#endif // TNAMEOBJECT_H
