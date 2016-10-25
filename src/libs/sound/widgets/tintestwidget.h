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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TINTESTWIDGET_H
#define TINTESTWIDGET_H


#include "nootkasoundglobal.h"
#include <QtWidgets/qdialog.h>


class TpitchView;
class QPushButton;
class QLabel;
class TnoteStruct;
class QSlider;


/**
 * @p TinTestWidget contains @p TpitchView and labels with detected pitch and frequency
 * It is used to quick settings of input sound under mobile
 */
class NOOTKASOUND_EXPORT TinTestWidget : public QDialog
{

  Q_OBJECT

public:
  explicit TinTestWidget(QWidget* parent = nullptr);
  virtual ~TinTestWidget();

  enum EexitMessage {
    e_discarded = 0, e_accepted = 1, e_audioSettings = 2
  };

  TpitchView* pitchView() { return m_pitchView; }

  void volUp();
  void volDown();

signals:
  void exit(int);


protected:
  void noteSlot(const TnoteStruct& ns);
  void exitSlot();
  void sysVolSlot();
  void volChangedSlot(int v);

  virtual void keyPressEvent(QKeyEvent* e);


private:
  TpitchView                *m_pitchView;
  QLabel                    *m_pitchLab, *m_freqLab;
  QPushButton               *m_sysVolButt, *m_exitButt;
  QSlider                   *m_outVolSlider;
};

#endif // TINTESTWIDGET_H
