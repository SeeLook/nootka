/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef NOOTINIWINDOW_H
#define NOOTINIWINDOW_H

#include <QtWidgets/QMainWindow>
#include <tartiniparams.h>

class NaudioLoader;
class QAction;
class Nchart;

/**
 * Main window of Nootini - pitch detection analyzer
 */
class NootiniWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit NootiniWindow(const QString& audioFile = "", QWidget* parent = 0);
  virtual ~NootiniWindow();

protected:
  void readConfig(); /** Reads other settings values than Nootka.conf from Nootini.conf */
  void writeConfig(); /** Writes other settings values to Nootini.conf */
  void startProcessing();

  virtual void closeEvent(QCloseEvent* e);
  virtual void resizeEvent(QResizeEvent* e);

protected slots:
  void openFileSlot();
  void settingsSlot();
  void processAudioFile(const QString& fileName);
  void processAgain();
  void zoom();
  void fitHeight();
  void saveAsSvg();

private:
  Nchart            *m_chart;
  QAction           *m_openAct, *m_settAct, *m_againAct, *m_toSvgAct;
  QAction           *m_zoomInAct, *m_zoomOutAct, *m_zoomFitAct;
  NaudioLoader      *m_loader;
  TartiniParams      m_tartiniParams;

};

#endif // NOOTINIWINDOW_H
