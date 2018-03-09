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

#ifndef TMELODYLISTVIEW_H
#define TMELODYLISTVIEW_H


#include <QtQuick/qquickitem.h>
#include <QtCore/qabstractitemmodel.h>


class TscoreObject;
class Tmelody;


/**
 * @todo docs
 */
// class TmelodyModel : public QAbstractListModel
// {
// 
//   Q_OBJECT
// 
// public:
//   enum EmelListRoles {
//     ScoreRole = Qt::UserRole + 1, MelodyRole
//   };
// 
//   TmelodyModel(QObject* parent = nullptr);
// 
//   void addMelody(Tmelody* melody, TscoreObject* score);
// 
//   QHash<int, QByteArray> roleNames() const override;
// };

class Tlevel;


/**
 * @todo write docs
 */
class TmelodyListView : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(int melodiesCount READ melodiesCount NOTIFY melodiesChanged)

public:
  explicit TmelodyListView(QQuickItem* parent = nullptr);
  ~TmelodyListView() override;

  int melodiesCount() const { return m_melodies.count(); }

  Q_INVOKABLE void setLevel(Tlevel* l);
  Q_INVOKABLE void save();

  Q_INVOKABLE void loadMelody();
  Q_INVOKABLE void removeMelody(int id);

  Q_INVOKABLE void setScore(int id, TscoreObject* score);

signals:
  void addScore();
  void melodiesChanged();
  void removeScore(int id);

protected:
  void loadMelodies();
  void clearMelodyList();

private:
  class TscoreMelody
  {
    public:
      TscoreMelody(TscoreObject* _score = nullptr, Tmelody *_melody = nullptr) : score(_score), melody(_melody) {}

      TscoreObject    *score = nullptr;
      Tmelody         *melody = nullptr;
      bool             delMelody = false; /**< if @p TRUE destructor will delete melody instance  */
  };

  Tlevel                    *m_level = nullptr;
  QVector<TscoreMelody>      m_melodies;
  bool                       m_listWasChanged = false;
  bool                       m_emitWhenRemove = true;
};

#endif // TMELODYLISTVIEW_H
