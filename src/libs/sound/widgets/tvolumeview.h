/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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


#ifndef TVOLUMEVIEW_H
#define TVOLUMEVIEW_H

#include "nootkacoreglobal.h"
#include "tabstractsoundview.h"


class NOOTKASOUND_EXPORT TvolumeView : public TabstractSoundView
{

Q_OBJECT

public:
  explicit TvolumeView(QWidget *parent = 0);
  virtual ~TvolumeView();

      /** @param alpha is alpha value of background color
      * to manage of the animation of detected pitch */
  void setVolume(float vol, int alpha = 0);
  void setPitchColor (QColor col) { m_pitchColor = col; }
  void setMinimalVolume(float vol) { m_minVolume = vol; update(); }

      /** Determines whether the view displays button when mouse hovers on it to pause/start pitch detection */
  void setPauseActive(bool active);
  bool isPauseActive() const { return m_activePause; }

  bool isPaused() const { return m_paused; }
  void setPaused(bool p) { m_paused = p; update(); }

      /** Sets visibility of the knob for volume  */
  void setKnobAlwaysVisible(bool vis) { m_knobAlwaysVisible = vis; update(); }
  bool knobAlwaysVisible() const { return m_knobAlwaysVisible; }

  virtual void setDisabled(bool isDisabled);

signals:
  void minimalVolume(float);

      /** Emitted when pause is active and it was clicked. */
  void paused();

protected:
  virtual void paintEvent(QPaintEvent* );
  virtual void resizeEvent(QResizeEvent*);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void enterEvent(QEvent* );
  virtual void leaveEvent(QEvent* );

private:
  float           m_volume, m_prevVol;
  QColor          m_pitchColor;
  int             m_alpha; // alpha value of m_pitchColor
  QList<QColor>   m_tickColors;
  int             m_ticksCount;
  int             m_noteWidth;
  float           m_hiTickStep;
  float           m_minVolume; // tick points minimal vol for pitch detection
  bool            m_drawKnob, m_leftButton, m_knobAlwaysVisible;
  bool            m_paused, m_activePause;
  bool            m_overNote, m_drawPaused;
  QFont           m_fontOfMinVol;
  int             m_widthOfMinVol;
};

#endif // TVOLUMEVIEW_H
