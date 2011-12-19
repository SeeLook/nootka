/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

#ifndef TVOLUMEMETER_H
#define TVOLUMEMETER_H

#include <QWidget>


/** Volume meter is volume meter. What we can say more ??
 * resize() method has to be called from outside. */
class TvolumeMeter : public QWidget
{
  Q_OBJECT
  
public:
  
  explicit TvolumeMeter(QWidget *parent = 0);
  virtual ~TvolumeMeter();
    
    /** @param alpha is alpha value of background color
     * to manage of the animation of detected pitch */
  void setVolume(qreal vol, int alpha = 0);
  void setPitchColor (QColor col) { m_pitchColor = col; }
    /** Level bar is painted in gray color till this value */
  void setNoiseLevel(float level) { m_noiseLevel = level; }

protected:
  void paintEvent(QPaintEvent* );

private:  
  qreal m_volume;
  QColor m_pitchColor;
  int m_alpha; // aplha value of m_pitchColor
  float m_noiseLevel; // 0 - 100 - the same as in TaudioParams
	
};

#endif // TVOLUMEMETER_H
