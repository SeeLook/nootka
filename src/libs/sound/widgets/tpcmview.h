/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#ifndef TPCMVIEW_H
#define TPCMVIEW_H


#include "nootkasoundglobal.h"
#include <QtWidgets/qwidget.h>


/**
 * Displays simple thin bar line with PCM input volume.
 * Volume is set through @p setPCMvolume, then bar is repainted
 */
class NOOTKASOUND_EXPORT TpcmView : public QWidget
{

  Q_OBJECT

public:
  explicit TpcmView(QWidget* parent = 0);

  void setPCMvolume(qreal vol);
  qreal pcmVolume() { return m_pcmVolume; }


protected:
  virtual void paintEvent(QPaintEvent*) override;
  virtual void resizeEvent(QResizeEvent*) override;

private:
  qreal                     m_pcmVolume;
  QLinearGradient           m_grad;

};

#endif // TPCMVIEW_H
