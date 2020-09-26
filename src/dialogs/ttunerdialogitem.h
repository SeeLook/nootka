/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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

#ifndef TTUNERDIALOGITEM_H
#define TTUNERDIALOGITEM_H


#include <QtQuick/qquickitem.h>


class QTimer;
class TnoteStruct;


/**
 * The logic of Nooter (Nootka tuner)
 * It saves changes of user action over middle A frequency QML spin box
 * to immediately adjust pitch detection of the tuner
 * and when dialog is closed, it saves that frequency globally
 */
class TtunerDialogItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(qreal deviation READ deviation NOTIFY deviationChanged)
  Q_PROPERTY(qreal frequency READ frequency NOTIFY frequencyChanged)
  Q_PROPERTY(QString noteName READ noteName NOTIFY noteNameChanged)
  Q_PROPERTY(QStringList tuningModel READ tuningModel NOTIFY tuningModelChanged)
  Q_PROPERTY(int workFreq READ workFreq WRITE setWorkFreq NOTIFY workFreqChanged)
  Q_PROPERTY(int pitch READ pitch NOTIFY noteNameChanged)

public:
  explicit TtunerDialogItem(QQuickItem* parent = nullptr);
  ~TtunerDialogItem() override;

  qreal deviation() const;
  qreal frequency() const { return m_frequency; }
  QString noteName() const;
  QStringList tuningModel();

  int workFreq() const { return m_workFreq; }
  void setWorkFreq(int wFreq);

  int pitch() const;

      /**
       * Lowest note in actual tuning - chromatic number
       */
  Q_INVOKABLE int lowestNote() const;
  Q_INVOKABLE int highestNote() const;
  Q_INVOKABLE QString styledName(int chroma) const;
  Q_INVOKABLE bool isOpenString(int chroma) const;

      /**
       * Returns real string number [1 - 6] of a given note
       * or @p 0 if not such a string
       */
  Q_INVOKABLE int whichString(int chroma) const;

signals:
  void deviationChanged();
  void frequencyChanged();
  void noteNameChanged();
  void tuningModelChanged();
  void workFreqChanged();

private:
  void delayedInit();
  void timeoutSlot();
  void noteStartedSlot(const TnoteStruct& note);

private:
  QTimer            *m_timer;
  bool               m_stoppedByUserState;
  qreal              m_frequency = 0.0;
  int                m_workFreq = 440;
};

#endif // TTUNERDIALOGITEM_H
