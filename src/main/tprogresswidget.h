/***************************************************************************
 *   Copyright (C) 2012-2017 by Tomasz Bojczuk                             *
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


#ifndef TPROGRESSWIDGET_H
#define TPROGRESSWIDGET_H


#include <nootkacoreglobal.h>
#include <QtQuick/qquickitem.h>


class Texam;


#define   PROGRESS    TprogressWidget::instance()


/**
 * Displays progress bar of exam and some additional data about it.
 * It should be activated by @p activate(),
 * then it is updated by @p progress() after every answer
 * and terminated with @p terminate() at the end..
 * If exam becomes passed (finished), invoke @p setFinished() to update.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p PROGRESS
 */
class NOOTKACORE_EXPORT TprogressWidget : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QString answersText READ answersText NOTIFY valuesUpdated)
  Q_PROPERTY(QString totalText READ totalText NOTIFY valuesUpdated)
  Q_PROPERTY(int progressValue READ progressValue NOTIFY valuesUpdated)
  Q_PROPERTY(int progressMax READ progressMax NOTIFY valuesUpdated)

public:
  explicit TprogressWidget(QQuickItem *parent = nullptr);
  ~TprogressWidget() override;

  QString answersText() const { return m_answersText; }
  QString totalText() const { return m_totalText; }
  int progressValue() const { return m_progressValue; }
  int progressMax() const { return m_progressMax; }

  static TprogressWidget* instance() { return m_instance; }

  static QString progressExamTxt() { return tr("Progress of the exam"); } // Progress of the exam
  static QString examFinishedTxt() { return tr("Exam was finished"); } // Exam was finished

  void activate(Texam* exam, int totalNr);
  void progress();
  void terminate();
  void setFinished();

//   void resize(int fontSize);

signals:
  void valuesUpdated();

protected:
//   QString zeroLabTxt();
  void updateLabels();

private:
  int                        m_totalNr;
  Texam                     *m_exam = nullptr;
  QString                    m_answersText, m_totalText;
  int                        m_progressValue, m_progressMax;
  static TprogressWidget    *m_instance;
};

#endif // TPROGRESSWIDGET_H
