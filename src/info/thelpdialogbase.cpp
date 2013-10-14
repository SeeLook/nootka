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
#include <QTextEdit>
#include <QApplication>
#include <QLayout>



ThelpDialogBase::ThelpDialogBase(QWidget* parent, Qt::WindowFlags f) :
  QDialog(parent, f),
  m_doNotShowChB(0),
  m_cancelButton(0)
{
  m_helpText = new QTextEdit(this);
  m_helpText->setReadOnly(true);
  m_helpText->setAlignment(Qt::AlignCenter);
  
  m_lay = new QVBoxLayout;
  m_lay->addWidget(m_helpText);
  m_buttonsLay = new QVBoxLayout;
  setLayout(m_lay);
}


void ThelpDialogBase::showCheckBox(const QString& label) {
  if (!m_doNotShowChB) {
    m_doNotShowChB = new QCheckBox(label, this);
    m_lay->insertWidget(1, m_doNotShowChB); // insert after m_helpText
  }
}


void ThelpDialogBase::showButtons(bool withOk, bool withCancel) {
  
}








