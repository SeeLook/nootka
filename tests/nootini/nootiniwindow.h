/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#include <qt/QtWidgets/QMainWindow>

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


protected slots:
  void openFileSlot();
  void processAudioFile(const QString& fileName);

private:
  Nchart            *m_chart;
  QAction           *m_openAct;
  NaudioLoader      *m_loader;

};

#endif // NOOTINIWINDOW_H
