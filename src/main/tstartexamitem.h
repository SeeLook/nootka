/***************************************************************************
 *   Copyright (C) 2011-2019 by Tomasz Bojczuk                             *
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

#ifndef TSTARTEXAMITEM_H
#define TSTARTEXAMITEM_H


#include <exam/tlevel.h>
#include <QtQuick/qquickitem.h>


class Tlevel;
class Taction;


class TstartExamItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QList<QObject*> recentModel READ recentModel NOTIFY recentModelChanged)
  Q_PROPERTY(QString lastExamFile READ lastExamFile NOTIFY lastExamFileChanged)

public:
  explicit TstartExamItem(QQuickItem* parent = nullptr);

  QList<QObject*> recentModel() { return m_recentModel; }

  QString lastExamFile() const;

  Q_INVOKABLE QString prevLevelName() const;

  Q_INVOKABLE Tlevel* prevLevel() { return m_prevExerciseLevel; }

      /**
       * Displays message box with that communicate.
       */
  Q_INVOKABLE void giveUserNameMessage();

      /**
       * Help text (HTML) about what is exercise and exam
       */
  static QString exerOrExamHelpTxt();

  Q_INVOKABLE void getHelpDialog();

signals:
  void recentModelChanged();
  void lastExamFileChanged();
  void openActChanged();
  void continueExam(const QString& examFile);

private:
  void examFromFileDialog();

      /**
       * when some action with exam file path was clicked.
       */
  void continuePrevExam();

      /**
       * Continue last exam button click slot
       */
  void continueTheLast();
  void examToContSelected(const QString& eFile);
  void levelWasSelected(const Tlevel& level);

private:
  QStringList                m_recentExams;
  QString                    m_selectedExamFile;
  QList<QObject*>            m_recentModel;
  Tlevel                    *m_prevExerciseLevel;

};

#endif // TSTARTEXAMITEM_H

