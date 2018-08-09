/***************************************************************************
 *   Copyright (C) 2018 by Tomasz Bojczuk                                  *
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

#ifndef TMELGENITEM_H
#define TMELGENITEM_H

#include <QtQuick/qquickitem.h>


class TrtmSelectorItem;
class QSettings;


/**
 * @class TmelGenItem is C++ logic of QML accessible @p MElGenDialog.qml
 * It reads and stores melody generation parameters from/into Nootka configuration
 */
class TmelGenItem : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(TrtmSelectorItem* rhythmSelector READ rhythmSelector WRITE setRhythmSelector)
  Q_PROPERTY(int barNumber READ barNumber WRITE setBarNumber NOTIFY barNumberChanged)
  Q_PROPERTY(int rhythmDiversity READ rhythmDiversity WRITE setRhythmDiversity NOTIFY rhythmDiversityChanged)
  Q_PROPERTY(bool endsOnTonic READ endsOnTonic WRITE setEndsOnTonic NOTIFY endsOnTonicChanged)
  Q_PROPERTY(bool onlyCurrKey READ onlyCurrKey WRITE setOnlyCurrKey NOTIFY onlyCurrKeyChanged)
  Q_PROPERTY(int maxStep READ maxStep WRITE setMaxStep NOTIFY maxStepChanged)

public:
  explicit TmelGenItem(QQuickItem* parent = nullptr);

  ~TmelGenItem() override;

  TrtmSelectorItem* rhythmSelector() { return m_rtmSelector; }
  void setRhythmSelector(TrtmSelectorItem* rtmS);

  int barNumber() const { return m_barNumber; }
  void setBarNumber(int bars);

  int rhythmDiversity() const { return m_rtmDiversity; }
  void setRhythmDiversity(int diverity);

  bool endsOnTonic() const { return m_endsOnTonic; }
  void setEndsOnTonic(bool eot);

  bool onlyCurrKey() const { return m_onlyCurrKey; }
  void setOnlyCurrKey(bool only);

  int maxStep() const { return m_maxStep; }
  void setMaxStep(int max);

  Q_INVOKABLE void generate();

signals:
  void barNumberChanged();
  void rhythmDiversityChanged();
  void endsOnTonicChanged();
  void onlyCurrKeyChanged();
  void maxStepChanged();

private:
  TrtmSelectorItem              *m_rtmSelector = nullptr;
  QSettings                     *m_settings;
  int                            m_rtmDiversity, m_barNumber;
  quint32                        m_basicMask, m_dotsMask;
  bool                           m_endsOnTonic, m_onlyCurrKey;
  int                            m_maxStep;
};

#endif // TMELGENITEM_H
