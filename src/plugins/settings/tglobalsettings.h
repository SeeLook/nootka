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


#ifndef TGLOBALSETTIGS_H
#define TGLOBALSETTIGS_H


#include <QtCore/QMap>
#include <touch/ttoucharea.h>

class TpluginsLoader;
class QLabel;
class QPushButton;
class QComboBox;
class TcolorButton;
class QCheckBox;


class TglobalSettings : public TtouchArea
{
    Q_OBJECT
public:
	explicit TglobalSettings(QWidget *parent = 0);

	void saveSettings();
	void restoreDefaults();
	static QString warringResetConfigTxt() {  return tr("All settings will be reset to their default values!<br>Nootka will start up with the first-run wizard."); } // All settings will be reset to their default values!<br>Nootka will start up next time with the first-run wizard.
	
signals:
	void restoreAllDefaults(); /** Is emitted when user click m_restAllDefaultsBut QPushButton. */
		
private:
	QComboBox 									*m_langCombo;
	QMap<QString, QString> 			 m_langList;
  QPushButton                 *m_restAllDefaultsBut;
#if !defined (Q_OS_ANDROID)
  QPushButton 								*m_updateButton;
  QLabel 											*m_updateLabel;
	TpluginsLoader							*m_pluginLoader;
#endif
  
private:
#if !defined (Q_OS_ANDROID)
  void updateSlot();
  void processOutputSlot(QString output);
#endif
	void restoreRequired(); /** when user wants to restore all Nootka settings at once */
};


#endif // TGLOBALSETTIGS_H
