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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef NAMESETTINGS_H
#define NAMESETTINGS_H

#include "tnotationradiogroup.h"
#if defined (Q_OS_ANDROID)
  #include <touch/ttoucharea.h>
#endif

class TcolorButton;
class TscalePreviewLabel;
class QCheckBox;
class Select7note;


/**
 * @class TnoteNameSettings derives from @class QWidget under desktop
 * because whole score settings is wrapped into @class TtouchArea.
 * Under mobile, every score settings page derives directly from @class TtouchArea.
 */

#if defined (Q_OS_ANDROID)
class TnoteNameSettings : public TtouchArea
#else
class TnoteNameSettings : public QWidget
#endif
{
    Q_OBJECT
public:
    explicit TnoteNameSettings(QWidget *parent = 0);

    void saveSettings();
		bool is7th_b();
		void restoreDefaults();

signals:
    void seventhIsBChanged(bool isB);

protected:
    void seventhNoteWasChanged(bool isB);
		void nameStyleWasChanged(Tnote::EnameStyle style);



private:
    TnotationRadioGroup 	*m_nameStyleGr;
    QCheckBox 						*m_octInNameCh, *m_nameInScoreChB;
    Select7note 					*m_select7;
		TscalePreviewLabel		*m_scaleLabel;
		TcolorButton					*m_nameColorButt;
};
#endif // NAMESETTINGS_H
