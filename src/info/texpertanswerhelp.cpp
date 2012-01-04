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
#include "examsettings.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QCheckBox>

TexpertAnswerHelp::TexpertAnswerHelp(bool& showHelp, QWidget* parent) :
    QDialog(parent),
    m_show(showHelp)
{
  setMaximumSize((parent->width()/3)*2, (parent->height()/3)*2);
  setWindowTitle(tr("Experts' answers"));
  
  
  QVBoxLayout *lay = new QVBoxLayout();
  QTextEdit *ed = new QTextEdit(tr("You are about to go in expert's answers.<br> In this mode You don't need to confirm every answer,<br><b>but remember the folowing:</b>") + "<ul><li>" + 
    tr("Selecting a note on the score or position on the fingerboard invokes checking of Your answer, so select a key signature first if required.") + "</li><li>" +
    tr("When an answer is name of a note <b>first select</b> a proper accidental and an octave and then click a note button - it invokes checking.") + "</li><li>" +
    tr("When You have to play a note as an answer - the first detected sound will be taken. Be sure that Your input device captures exacly what You want.") + "<br><br>" +
    tr("That's all. You become a master soon...")
    , this);
  ed->setReadOnly(true);
  ed->setFixedSize((parent->width()/3)*2, (parent->height()/5)*3);
  ed->setAlignment(Qt::AlignCenter);
  lay->addWidget(ed);
  
  showInfoChB = new QCheckBox(tr("remind me about this"), this);
  lay->addWidget(showInfoChB, 0, Qt::AlignCenter);
  showInfoChB->setChecked(m_show); 
  
  QPushButton *okBut = new QPushButton(tr("OK"),  this);
  lay->addWidget(okBut, 0, Qt::AlignCenter);
  
  setLayout(lay);
  
  connect(okBut, SIGNAL(clicked()), this, SLOT(closeIt()));

}

void TexpertAnswerHelp::closeIt() {
  m_show = showInfoChB->isChecked();
  close();
}

