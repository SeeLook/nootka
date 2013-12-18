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


#include "tfixlevelwidget.h"
#include "tselectinstrument.h"
#include <QLayout>
#include <QLabel>

TfixLevelWidget::TfixLevelWidget(QWidget* parent) :
	QDialog(parent)
{ 
	m_selInstr = new TselectInstrument(this, TselectInstrument::e_textUnderButtonH);
	m_selInstr->setHeadLabel(tr("Due to bug in Nootka version 0.8.95 kinds of instrument were stored wrongly.<br>Please, select an instrument for what this level/exam file was created."));
	QVBoxLayout *lay = new QVBoxLayout;
	lay->addWidget(m_selInstr);
	setLayout(lay);
}


