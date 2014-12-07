/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TLAYSETTINGS_H
#define TLAYSETTINGS_H

#include <QWidget>

class TlayoutParams;
class QRadioButton;
class QCheckBox;
class QGroupBox;


/** 
 * Options for widgets layout in main Nootka window
 */
class TlaySettings : public QWidget
{

	Q_OBJECT
	
public:
	
	TlaySettings(TlayoutParams* layParams, QWidget* parent = 0);
	
	void saveSettings();
	void restoreDefaults();

protected:
	virtual void resizeEvent(QResizeEvent* );

private:
	TlayoutParams			*m_layParams;
	QGroupBox					*m_toolBox, *m_hintBox, *m_soundBox, *m_scoreBox, *m_guitarBox;
	QCheckBox					*m_barAutoHideChB;
	QRadioButton			*m_textUnderRadio, *m_iconsOnlyRadio, *m_textOnlyRadio;

};

#endif // TLAYSETTINGS_H
