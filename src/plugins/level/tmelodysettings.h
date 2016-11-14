/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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

#ifndef TMELODYSETTINGS_H
#define TMELODYSETTINGS_H

#include "tabstractlevelpage.h"

class QGroupBox;
class TlevelCreatorDlg;
class Tlevel;
class QCheckBox;
class QSpinBox;
class QRadioButton;
class TmultiScore;

/**
 * Settings for level with melodies
 */
class TmelodySettings : public TabstractLevelPage
{

Q_OBJECT

public:
  explicit TmelodySettings(TlevelCreatorDlg* creator);

  virtual void loadLevel(Tlevel* level);
  virtual void saveLevel(Tlevel* level);

  virtual void changed();

protected:
  virtual void changedLocal();

private:
  void fillLevelQA(Tlevel* l);

private:
  int                     m_tempMelLen; /**< Keeps the latest length of melody set. */
  QSpinBox               *m_melodyLengthSpin;
  QCheckBox              *m_finishOnChB, *m_equalTempoChB, *m_playMelodyChB, *m_writeMelodyChB;
  QGroupBox              *m_melGroup;
  QRadioButton           *m_randomRadio, *m_listRadio;
  TmultiScore            *m_score;
};

#endif // TMELODYSETTINGS_H
