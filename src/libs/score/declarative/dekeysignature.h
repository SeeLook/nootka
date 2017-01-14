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

#ifndef DEKEYSIGNATURE_H
#define DEKEYSIGNATURE_H


#include <QtQuick/qquickitem.h>


class DeScore;

class DeKeySignature : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(int key READ keySignature WRITE setKeySignature NOTIFY keySignatureChanged)

  friend class DeScore;

public:
  explicit DeKeySignature(QQuickItem* parent = nullptr);
  ~DeKeySignature();

  qint8 keySignature() { return m_key; }
  void setKeySignature(qint8 k);

signals:
  void keySignatureChanged();

protected:
  void setScore(DeScore* score) { m_score = score; }

private:
  qint8       m_key;
  DeScore    *m_score;

};

#endif // DEKEYSIGNATURE_H
