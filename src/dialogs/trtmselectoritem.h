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

#ifndef TRTMSELECTORITEM_H
#define TRTMSELECTORITEM_H


#include <QtQuick/qquickitem.h>
#include <music/trtmgroup.h>


#define DOTS_MASK         (16383)
#define BASIC_MASK        (4194303)


/**
 * C++ logic of QML RhythmmSelector control
 */
class TrtmSelectorItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(quint32 basicMask READ basicMask WRITE setBasicMask NOTIFY basicMaskChanged)
  Q_PROPERTY(quint32 dotsMask READ dotsMask WRITE setDotsMask NOTIFY dotsMaskChanged)

public:
  explicit TrtmSelectorItem(QQuickItem* parent = nullptr);

  ~TrtmSelectorItem() override;

  Q_INVOKABLE QString getGroupText(int rtmGrEnum);
  Q_INVOKABLE void userChangedGroup(int grId, bool checked);

  quint32 basicMask() const { return m_basicMask; }
  void setBasicMask(quint32 v);
  quint32 dotsMask() const { return m_dotsMask; }
  void setDotsMask(quint32 dm);

  static quint32 bitOf(qreal bitNr) { return qRound(qPow(2.0, bitNr)); }

signals:
  void basicMaskChanged();
  void dotsMaskChanged();
  void basicGroupChanged();
  void dotsGroupChanged();

private:
  quint32             m_basicMask = 0;
  quint32             m_dotsMask = 0;
};

#endif // TRTMSELECTORITEM_H
