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

#include "thelpdialogbase.h"
#include "tpixmaker.h"
#include <tglobals.h>
#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QStyle>

extern Tglobals *gl;


QString ThelpDialogBase::m_path = "";

ThelpDialogBase::ThelpDialogBase(QWidget* parent, Qt::WindowFlags f) :
  QDialog(parent, f),
  m_checkBox(0),
  m_cancelButton(0), m_OkButton(0),
  m_stateOfChB(0)
{
	m_path = gl->path;
	setWindowIcon(QIcon(path() + "picts/help.png"));
  setWindowTitle(tr("Nootka help"));
  m_helpText = new QTextEdit(this);
  m_helpText->setReadOnly(true);
  m_helpText->setAlignment(Qt::AlignCenter);
  
  m_lay = new QVBoxLayout;
  m_lay->addWidget(m_helpText);
  m_buttonsLay = new QHBoxLayout;
	m_buttonsLay->addStretch(1);
	m_buttonsLay->addStretch(1);
	m_lay->addLayout(m_buttonsLay);
  setLayout(m_lay);
	showButtons(true, false); // OK button by default
}


ThelpDialogBase::~ThelpDialogBase()
{
	if (m_stateOfChB && m_checkBox)
		*m_stateOfChB = m_checkBox->isChecked();
}


void ThelpDialogBase::showCheckBox(const QString& label, bool* state) {
  if (!m_checkBox) {
    m_checkBox = new QCheckBox(label, this);
    m_lay->insertWidget(1, m_checkBox, 0 , Qt::AlignCenter); // insert after m_helpText
  }
  m_stateOfChB = state;
  m_checkBox->setChecked(*state);
}


QString ThelpDialogBase::pix(const QString& imageName, int height) {
  return pixToHtml(path() + "picts/" + imageName + ".png", height);
}


void ThelpDialogBase::showButtons(bool withOk, bool withCancel) {
  if (withOk) {
		if (!m_OkButton) {
			m_OkButton = new QPushButton(tr("OK"), this);
			m_OkButton->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogOkButton)));
			if (m_cancelButton) {
				m_buttonsLay->insertStretch(3, 1);
			}
			m_buttonsLay->insertWidget(1, m_OkButton, 1);
			connect(m_OkButton, SIGNAL(clicked()), this, SLOT(accept()));
		}
  } else {
		if (m_OkButton) {
			delete m_OkButton;
			m_OkButton = 0;
		}
  }
  if (withCancel) {
		if (!m_cancelButton) {
			m_cancelButton = new QPushButton(tr("Discard"), this);
			m_cancelButton->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)));
			int pos = 1; // position to insert the button
			if (m_OkButton) {
				m_buttonsLay->insertStretch(2, 1);
				pos = 3;
			}
			m_buttonsLay->insertWidget(pos, m_cancelButton, 1);
			connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
		}
  } else {
		if (m_cancelButton) {
			delete m_cancelButton;
			m_cancelButton = 0;
		}
  }
}








