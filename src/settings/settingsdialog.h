/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "tnote.h"
#include "ttune.h"
#include "tscorewidgetsimple.h"
#include "tsettingsdialogbase.h"
#include "namesettings.h"
#include "scoresettings.h"
#include <QMap>

class AudioOutSettings;
class AudioInSettings;
class QCheckBox;
class QSpinBox;
class QComboBox;
class ExamSettings;
class TcolorButton;

class GlobalSettings : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalSettings(QWidget *parent = 0);

    void saveSettings();
private:
  QCheckBox *otherEnharmChBox, *dblAccChBox, *hintsEnabledChBox;
	TcolorButton *enharmColorBut;
	QComboBox *langCombo;
	QMap<QString, QString> langList;
  QPushButton *updateButton;
  QLabel* updateLabel;
  
private slots:
  void updateSlot();
  void processOutputSlot(QString output);
};



//############## GuitarSettings ################################################
class GuitarSettings : public QWidget
{
    Q_OBJECT
public:
    explicit GuitarSettings(QWidget *parent = 0);

    void saveSettings();

private:
    void setTune (Ttune tune);

    TscoreWidgetSimple *tuneView;
    QCheckBox *righthandCh, *morePosCh;
    QRadioButton *prefFlatBut, *prefSharpBut;
    QSpinBox *fretsNrSpin;
    QComboBox *tuneCombo;
    TcolorButton *pointColorBut, *selColorBut;

private slots:
    void tuneSelected(int tuneId);
    void userTune(int, Tnote);
};

//################ SettingsDialog ##############################################
class SettingsDialog : public TsettingsDialogBase
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

public slots:
    void saveSettings();

private:

    GlobalSettings *m_globalSett;
    ScoreSettings *m_scoreSett;
    NameSettings *m_nameSett;
    GuitarSettings *m_guitarSett;
    ExamSettings   *m_examSett;
    AudioOutSettings *m_sndOutSett;
    AudioInSettings *m_sndInSett;
    
private slots:
    void changeSettingsWidget(int index);

};

#endif // SETTINGSDIALOG_H
