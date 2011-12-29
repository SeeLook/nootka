/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

#include "texpertanswerhelp.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>

TexpertAnswerHelp::TexpertAnswerHelp(QWidget* parent) :
    QDialog(parent)
{
  setMaximumSize((parent->width()/3)*2, (parent->height()/3)*2);
  setWindowTitle(tr("Experts' answers"));
  
  
  QVBoxLayout *lay = new QVBoxLayout();
  QTextEdit *ed = new QTextEdit(tr("You are trying to switch on some advanced mode"), this);
  ed->setReadOnly(true);
  ed->setFixedSize((parent->width()/3)*2, (parent->height()/5)*3);
  ed->setAlignment(Qt::AlignCenter);
  lay->addWidget(ed);
  QPushButton *okBut = new QPushButton(tr("OK"),  this);
  lay->addWidget(okBut, 0, Qt::AlignCenter);
  
  setLayout(lay);
  
  connect(okBut, SIGNAL(clicked()), this, SLOT(close()));

}


