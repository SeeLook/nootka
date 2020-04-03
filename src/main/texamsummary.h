/***************************************************************************
 *   Copyright (C) 2011-2020 by Tomasz Bojczuk                             *
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

#ifndef TEXAMSUMMARY_H
#define TEXAMSUMMARY_H


#include <QtQuick/qquickitem.h>


class Texam;
class TlevelPreviewItem;


/**
 * Content of a dialog with exam summary
 * When @param cont is true On button is shown text 'continue'
 */
class TexamSummary : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(TlevelPreviewItem* levelPreview READ levelPreview WRITE setLevelPreview)
  // Exam properties
  Q_PROPERTY(QString student READ student NOTIFY updateExam)
  Q_PROPERTY(QString results READ results NOTIFY updateExam)
  Q_PROPERTY(QList<int> kindOfMistakes READ kindOfMistakes NOTIFY updateExam)
  Q_PROPERTY(bool hasVariousMistakes READ hasVariousMistakes NOTIFY updateExam)

public:
  explicit TexamSummary(QQuickItem* parent = nullptr);

  TlevelPreviewItem* levelPreview() { return m_levelPreview; }
  void setLevelPreview(TlevelPreviewItem* lp);

  QString student() const;
  QString results() const { return m_results; }

  QList<int> kindOfMistakes() { return m_kindOfMistakes; }

      /**
       * @p TRUE when there are more than one kind of mistake
       */
  bool hasVariousMistakes() { return m_kindOfMistakes.count() > 1; }

  Q_INVOKABLE QString title() const;

  Q_INVOKABLE bool isExercise() const;
  Q_INVOKABLE bool continueExecutor();
  Q_INVOKABLE void exerciseToExam();
  Q_INVOKABLE void closeSummary();

      /**
       * Indicates whether the summary dialog was discarded when exam is presented before start.
       * Summary dialog has to delete executor then.
       * In other cases (when exam or exercise was performed and executor was fully initialized)
       * the executor destroys itself
       */
  Q_INVOKABLE bool discardedAtBegin() const;

      /**
       * When 'continue' button can be displayed to user
       */
  Q_INVOKABLE bool enableContinue() const;

      /**
       * Number of columns to contain navigation buttons. (2 or 3)
       * To minimize QML parsing 
       */
  Q_INVOKABLE int buttColumsCount() const;

  Q_INVOKABLE Texam* exam() { return m_exam; }
#if defined (Q_OS_ANDROID)
  Q_INVOKABLE void sendExam();
#endif


signals:
  void updateExam();

private:
  Texam                       *m_exam = nullptr;
  TlevelPreviewItem           *m_levelPreview = nullptr;
  bool                         m_accepted = false;
  QString                      m_results;
  QList<int>                   m_kindOfMistakes;
};

#endif // TEXAMSUMMARY_H
