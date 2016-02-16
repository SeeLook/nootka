/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#include "thelpdialogbase.h"
#include <tpath.h>
#include <graphics/tnotepixmap.h>
#include <widgets/tsettingsdialogbase.h>
#include <QtWidgets/QtWidgets>


/*static*/
QString ThelpDialogBase::m_path = QString();

QString ThelpDialogBase::onlineDocP(const QString& hash) {
  return QString("<p align=\"right\"><a href=\"http://nootka.sourceforge.net/index.php?L=%1&C=doc#%2\">").arg(QString(qgetenv("LANG")).left(2).toLower()).arg(hash) + 
      TsettingsDialogBase::tr("Open online documentation") + "</a> </p>";
}


//##########################################################################################
//#######################     PUBLIC        ################################################
//##########################################################################################

ThelpDialogBase::ThelpDialogBase(QWidget* parent, Qt::WindowFlags f) :
  QDialog(parent, f),
  m_checkBox(0),
  m_stateOfChB(0)
{
	m_path = Tpath::main;
#if defined (Q_OS_ANDROID)
  showMaximized();
#else
	setWindowIcon(QIcon(Tpath::img("help")));
  setWindowTitle(tr("Nootka help"));
#endif
  m_helpText = new QTextBrowser(this);
		m_helpText->setReadOnly(true);
		m_helpText->setAlignment(Qt::AlignCenter);
    m_helpText->setOpenExternalLinks(true);
    m_helpText->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
		m_buttonBox->setCenterButtons(true);

  QScroller::grabGesture(helpText()->viewport(), QScroller::LeftMouseButtonGesture);
#if defined (Q_OS_ANDROID)
  helpText()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  helpText()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif

  m_lay = new QVBoxLayout;
  m_lay->addWidget(m_helpText);
	m_lay->addWidget(m_buttonBox);

  setLayout(m_lay);
	showButtons(true, false); // OK button by default

	connect(buttonBox(), SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox(), SIGNAL(rejected()), this, SLOT(reject()));
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
#if defined (Q_OS_ANDROID)
    m_lay->removeWidget(m_buttonBox);
    auto extraLay = new QHBoxLayout;
      extraLay->addStretch(2);
      extraLay->addWidget(m_checkBox);
      extraLay->addStretch(1);
      extraLay->addWidget(m_buttonBox);
      extraLay->addStretch(2);
    m_lay->addLayout(extraLay);
#endif
  }
  m_stateOfChB = state;
  m_checkBox->setChecked(*state);
}


QString ThelpDialogBase::pix(const char* imageName, int height) {
  return pixToHtml(Tpath::img(imageName), height);
}


void ThelpDialogBase::showButtons(bool withOk, bool withCancel) {
  if (withOk) {
		if (!m_OkButton) {
			m_OkButton = buttonBox()->addButton(QDialogButtonBox::Ok);
			m_OkButton->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogOkButton)));
		}
  } else {
			buttonBox()->removeButton(m_OkButton);
			delete m_OkButton;
  }
  if (withCancel) {
		if (!m_cancelButton) {
			m_cancelButton = buttonBox()->addButton(QDialogButtonBox::Cancel);
			m_cancelButton->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogCancelButton)));
		}
  } else {
			buttonBox()->removeButton(m_cancelButton);
			delete m_cancelButton;
  }
}








