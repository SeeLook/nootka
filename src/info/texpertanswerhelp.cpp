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
#include <QHBoxLayout>
#include <QTextEdit>
#include <QCheckBox>

TexpertAnswerHelp::TexpertAnswerHelp(bool& showHelp, QWidget* parent, bool showChBox) :
    QDialog(parent),
    m_show(showHelp),
    showInfoChB(0)
{
  setMaximumSize((parent->width()/3)*2, (parent->height()/2));
  setWindowTitle(tr("Experts' answers"));
  
  QVBoxLayout *lay = new QVBoxLayout();
  QTextEdit *ed = new QTextEdit(tr("You are about to go in expert's answers.<br> In this mode You don't need to confirm every answer,<br><b>but remember the folowing:</b>") + "<ul><li>" + 
    tr("Selecting a note on the score or position on the fingerboard invokes checking of Your answer, so select a key signature first if required.") + "</li><li>" +
    tr("When an answer is name of a note <b>first select</b> a proper accidental and an octave and then click a note button - it invokes checking.") + "</li><li>" +
    tr("When You have to play a note as an answer - the first detected sound will be taken. Be sure that Your input device captures exacly what You want.") + "<br><br>"
    , this);
  ed->setReadOnly(true);
  ed->setFixedSize((parent->width()/3)*2, (parent->height()/2));
  ed->setAlignment(Qt::AlignCenter);
  lay->addWidget(ed);
  
  if (showChBox) {
      showInfoChB = new QCheckBox(tr("Always remind me about this"), this);
      lay->addWidget(showInfoChB, 0, Qt::AlignCenter);
      showInfoChB->setChecked(m_show); 
  }
  QHBoxLayout *buttLay = new QHBoxLayout();
  buttLay->addStretch(1);
  QPushButton *appBut = new QPushButton(tr("Apply"),  this);
  buttLay->addWidget(appBut, 0, Qt::AlignCenter);
  buttLay->addStretch(1);
  QPushButton *discBut= new QPushButton(tr("Discard"),  this);
  buttLay->addWidget(discBut,  0, Qt::AlignCenter);
  buttLay->addStretch(1);
  lay->addLayout(buttLay);
  
  setLayout(lay);
  
  connect(appBut, SIGNAL(clicked()), this, SLOT(accept()));
  connect(discBut, SIGNAL(clicked()), this, SLOT(reject()));
  connect(this, SIGNAL(finished(int)), this, SLOT(closeIt()));
}

void TexpertAnswerHelp::closeIt() {
  if (showInfoChB)
      m_show = showInfoChB->isChecked();
  close();
}

bool showExpertAnswersHelpDlg(bool& showHelp, QWidget* parent, bool showChBox) {
  TexpertAnswerHelp *exHlp = new TexpertAnswerHelp(showHelp, parent, showChBox);
  bool res = false;
  exHlp->exec();
  if (exHlp->result() == QDialog::Accepted)
      res = true;    
  delete exHlp;
  return res;
}
