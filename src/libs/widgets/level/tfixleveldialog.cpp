/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#include "tfixleveldialog.h"
#include "widgets/tselectinstrument.h"
#include "widgets/troundedlabel.h"
#include "exam/tlevel.h"
#include <QtWidgets>


bool fixLevelInstrument(Tlevel& level, const QString& fileName, int& autoFixInstr, QWidget* parent) {
	bool result = false;
	if (level.hasInstrToFix) {
		int fixedValue = -1;
		if (autoFixInstr == -1) { // display dialog 
			bool isAuto = false;
			int prefInstr;
			TfixLevelDialog *fixDialog = new TfixLevelDialog(level, fileName, isAuto, prefInstr, parent);
			if (fixDialog->exec() == QDialog::Accepted) {
				level.instrument = (Einstrument)prefInstr; // save selected instrument in the level
				fixedValue = prefInstr;
				if (isAuto) // store user preference about auto fixing 
						autoFixInstr = prefInstr; // when true it takes instrument value otherwise reminds -1  
				result = true;
			}
			delete fixDialog;
		} else { // fix a level automatically
				level.instrument = Einstrument(autoFixInstr);
				fixedValue = autoFixInstr;
				result = true;
		}
		if (fixedValue > -1) {
			level.hasInstrToFix = false;
			qDebug() << "Instrument in level:" << level.name << "fixed to:" << instrumentToText(Einstrument(fixedValue));
		}
	} else
			result = true;
	return result;
}

//*********************************************************************************************************

TfixLevelDialog::TfixLevelDialog(Tlevel &level, const QString& fileName, bool& autoFix, int& prefInstr, QWidget* parent) :
	QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	m_autoFix(autoFix),
	m_instr(prefInstr)
{ 
	setWindowTitle("fix an instrument");
	m_selInstr = new TselectInstrument(this, TselectInstrument::e_textUnderButtonH);
	QString fN = "";
	if (fileName != "") 
		fN = "<br><b>(" + fileName + ")</b>";
	m_selInstr->setHeadLabel(QString("Due to bug in <b>Nootka version 0.8.95</b> instrument type can be incorrect.<br>Please, select a correct instrument for the level %1").arg(":<br><u><b>" + level.name + "</b></u>") + fN);
	m_selInstr->setInstrument((int)level.instrument);
	m_autoCheckBox = new QCheckBox("fix levels automatically", this);
	m_autoCheckBox->setChecked(m_autoFix);
	TroundedLabel *autoLab = new TroundedLabel("When you are sure that <b>all your Nootka files</b> were created exactly for selected instrument<br>you can check this box and<br><b>all incorrect files will be fixing automatically.</b>", this);
	autoLab->setAlignment(Qt::AlignCenter);
	autoLab->setWordWrap(true);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
	QPushButton *okBut = buttonBox->addButton(QDialogButtonBox::Ok);
		okBut->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
	QPushButton *cancelBut = buttonBox->addButton(QDialogButtonBox::Cancel);
		cancelBut->setIcon(style()->standardIcon(QStyle::SP_DialogDiscardButton));
	QVBoxLayout *lay = new QVBoxLayout;
		lay->addWidget(m_selInstr);
		lay->addStretch();
		QVBoxLayout *fixAutoLay = new QVBoxLayout;
			fixAutoLay->addWidget(m_autoCheckBox, 0, Qt::AlignCenter);
			fixAutoLay->addWidget(autoLab);
			QGroupBox *fixAutoGr = new QGroupBox(this);
			fixAutoGr->setLayout(fixAutoLay);
		lay->addWidget(fixAutoGr);	
		lay->addStretch();
		lay->addWidget(buttonBox);
	setLayout(lay);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(okSlot()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));	
}


void TfixLevelDialog::okSlot() {
	m_instr = m_selInstr->instrument();
	m_autoFix = m_autoCheckBox->isChecked();
	accept();
}






