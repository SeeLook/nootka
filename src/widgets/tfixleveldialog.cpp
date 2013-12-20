/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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
#include "tselectinstrument.h"
#include "troundedlabel.h"
#include "tlevel.h"
#include <QtGui>


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
	setWindowTitle(tr("fix an instrument"));
	m_selInstr = new TselectInstrument(this, TselectInstrument::e_textUnderButtonH);
	QString fN = "";
	if (fileName != "") 
		fN = "<br><b>(" + fileName + ")</b>";
	m_selInstr->setHeadLabel(tr("Due to bug in <b>Nootka version 0.8.95</b> instrument type can be incorrect.<br>Please, select a correct instrument for the level %1").arg(":<br><u><b>" + level.name + "</b></u>") + fN);
	m_selInstr->setInstrument((int)level.instrument);
	m_autoCheckBox = new QCheckBox(tr("fix levels automatically"), this);
	m_autoCheckBox->setChecked(m_autoFix);
	TroundedLabel *autoLab = new TroundedLabel(tr("When you are sure that <b>all your Nootka files</b> were created exactly for selected instrument<br>you can check this box and<br><b>all incorrect files will be fixing automatically.</b>"), this);
	autoLab->setAlignment(Qt::AlignCenter);
	autoLab->setWordWrap(true);
	QPushButton *okButt = new QPushButton(QIcon(style()->standardIcon(QStyle::SP_DialogOkButton)), tr("OK"), this);
	QPushButton *cancleButt = new QPushButton(QIcon(style()->standardIcon(QStyle::SP_DialogCancelButton)), tr("Cancel"), this);
	
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
		QHBoxLayout *buttLay = new QHBoxLayout;
			buttLay->addStretch();
			buttLay->addWidget(okButt);
			buttLay->addStretch();
			buttLay->addWidget(cancleButt);
			buttLay->addStretch();
		lay->addLayout(buttLay);
	setLayout(lay);
	connect(okButt, SIGNAL(clicked()), this, SLOT(okSlot()));
	connect(cancleButt, SIGNAL(clicked()), this, SLOT(reject()));
}


void TfixLevelDialog::okSlot() {
	m_instr = m_selInstr->instrument();
	m_autoFix = m_autoCheckBox->isChecked();
	accept();
}






