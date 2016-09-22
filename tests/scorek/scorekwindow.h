/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#ifndef SCOREKWINDOW_H
#define SCOREKWINDOW_H

#include <QtWidgets/qmainwindow.h>


class TmultiScore;
class PianoKeybd;


class ScorekWindow : public QMainWindow
{

  Q_OBJECT

public:
  ScorekWindow(QWidget* parent = 0);

protected:
  void keySignatureSlot(bool keyEnabled);
  void meterSlot(bool meterEnabled);
  void dblAccidsSlot(bool dblAccidsEnabled);
  void randomSlot();

  virtual void resizeEvent(QResizeEvent* event) override;


private:
  TmultiScore           *m_score;
  PianoKeybd            *m_piano;
};

#endif // SCOREKWINDOW_H
