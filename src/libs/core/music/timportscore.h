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

#ifndef TIMPORTSCORE_H
#define TIMPORTSCORE_H


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


#define IMPORT_SCORE TimportScore::instance()


class Tmelody;
class Tchunk;
class TscoreObject;


class NOOTKACORE_EXPORT TmelodyPart : public QObject
{

  Q_OBJECT

  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
  Q_PROPERTY(int part READ part NOTIFY melodyChanged)
  Q_PROPERTY(int staff READ staff NOTIFY melodyChanged)
  Q_PROPERTY(int voice READ voice NOTIFY melodyChanged)
  Q_PROPERTY(int splitBarNr READ splitBarNr WRITE setSplitBarNr NOTIFY splitBarNrChanged)
  Q_PROPERTY(QList<TmelodyPart*> snippets READ snippets NOTIFY melodyChanged)

public:
  explicit TmelodyPart (TmelodyPart* parent = nullptr, int partId = 0, int staffNr = 0, int voiceNr = 0);
    ~TmelodyPart() override;

  bool selected() const { return m_selected; }
  void setSelected(bool sel);
  int part() const { return m_partId; }
  int staff() const { return m_staffNr; }
  int voice() const { return m_voiceNr; }
  int splitBarNr() const { return m_splitBarNr; }
  void setSplitBarNr(int splitNr);

  Tmelody* melody() { return m_melody; }
  void setMelody(Tmelody* m);

  int count() const { return parts.count(); }

  QList<TmelodyPart*>    parts;
  QList<TmelodyPart*>    snippets() { return parts; }

  Q_INVOKABLE void setScoreObject(TscoreObject* sObj);

signals:
  void melodyChanged();
  void selectedChanged();
  void splitBarNrChanged();

private:
  int                   m_partId = 0;
  int                   m_staffNr = 0;
  int                   m_voiceNr = 0;
  int                   m_snippet = 0;
  Tmelody              *m_melody = nullptr;
  TscoreObject         *m_scoreObj = nullptr;
  bool                  m_selected = false;
  int                   m_splitBarNr = 0;
};


/**
 * @todo write docs
 */
class NOOTKACORE_EXPORT TimportScore : public QObject
{

  Q_OBJECT

  friend class TmelodyPart;

public:
  TimportScore(Tmelody* melody, QObject *parent = nullptr);
  ~TimportScore();

  static TimportScore* instance() { return m_instance; }

  void addNote(int partId, int staff, int voice, const Tchunk& note);

  void sumarize();

  Tmelody* mainMelody() { return m_melody; }

  QList<TmelodyPart*>* parts() { return &m_parts; }

  QList<QObject*> model() { return m_partsModel; }

      /**
       * @p TRUE when there are more melodies to import
       */
  bool hasMoreParts() const { return m_hasMoreParts; }

  static int splitEveryBarNr() { return m_splitEveryBarNr; }
  static void setSplitBarNr(int splitNr) { m_splitEveryBarNr = splitNr; }

signals:
  void importReady();

private:
  static TimportScore        *m_instance;
  QList<TmelodyPart*>         m_parts;
  QList<QObject*>             m_partsModel;
  bool                        m_hasMoreParts = false;
  Tmelody                    *m_melody; /**< Main melody */
  static int                  m_splitEveryBarNr;
};

#endif // TIMPORTSCORE_H
