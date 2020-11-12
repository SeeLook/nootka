/***************************************************************************
 *   Copyright (C) 2020 by Tomasz Bojczuk                                  *
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

#ifndef TMELODYWRAPPER_H
#define TMELODYWRAPPER_H


#include <QtQuick/qquickitem.h>


class Tmelody;
class TmelodyListView;
class TscoreObject;


/**
 * Logic of QML @p MelodyWrapper.
 * Exposes some melody info, sets melody to @p TscoreObject.
 */
class TmelodyWrapper : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(int nr READ nr WRITE setNr NOTIFY nrChanged)
  Q_PROPERTY(TmelodyListView* melodyView READ melodyView WRITE setMelodyView)
  Q_PROPERTY(TscoreObject* score READ score WRITE setScore)
  Q_PROPERTY(QString title READ title NOTIFY melodyChanged)
  Q_PROPERTY(QString composer READ composer NOTIFY melodyChanged)

public:
  explicit TmelodyWrapper(QQuickItem* parent = nullptr);
  ~TmelodyWrapper();

  int nr() const { return m_nr; }
  void setNr(int n);

  QString title() const;
  QString composer() const;

  TmelodyListView* melodyView() { return m_melodyView; }
  void setMelodyView(TmelodyListView* mv) { 
        m_melodyView = mv; }

  TscoreObject* score() { return m_score; }
  void setScore(TscoreObject* sc) { m_score = sc; }

      /**
       * Gets @p Tmelody from melody view by @p nr()
       * and sets to @p TscoreObject.
       * Refreshes title and composer by emitting @p melodyChanged()
       */
  Q_INVOKABLE void updateMelody();

signals:
  void nrChanged();
  void melodyChanged();

private:
  int                     m_nr = 0;
  TmelodyListView        *m_melodyView = nullptr;
  Tmelody                *m_melody = nullptr;
  bool                    m_deleteMelody = false;
  TscoreObject           *m_score = nullptr;
};

#endif // TMELODYWRAPPER_H
