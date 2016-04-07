/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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


#include "tlevelheaderwdg.h"
#include <qtr.h>
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
#endif
#include <QtWidgets/QtWidgets>


TlevelHeaderWdg::TlevelHeaderWdg(QWidget *parent) :
  QDialog(parent)
{
  setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog);
  auto mainLay = new QVBoxLayout;
  auto nameLab = new QLabel(tr("Level name:"), this);
  mainLay->addWidget(nameLab);
  nameEd = new QLineEdit(this);
  nameEd->setMaxLength(25);
  nameEd->setText(tr("new level"));
  mainLay->addWidget(nameEd);
  QLabel *descLab = new QLabel(tr("Level description:"), this);
  mainLay->addWidget(descLab);
  descEd = new QTextEdit(this);
  descEd->setFixedHeight(fontMetrics().boundingRect(QStringLiteral("A")).height() * 4);
  descEd->setFixedWidth(fontMetrics().boundingRect(QStringLiteral("w")).width() * 30);
  descEd->setLineWrapMode(QTextEdit::FixedColumnWidth);
  descEd->setLineWrapColumnOrWidth(35);
  mainLay->addWidget(descEd);
  okBut = new QPushButton(qTR("QDialogButtonBox", "OK"), this);
  mainLay->addWidget(okBut, 1, Qt::AlignCenter);

  setLayout(mainLay);

#if defined (Q_OS_ANDROID)
  move((Tmtr::screenWidth() - descEd->width()) / 2, Tmtr::fingerPixels() / 2);
  // for better visibility where there is no window frame on mobile change background color
  setObjectName(QStringLiteral("levelHeader"));
  setStyleSheet(QStringLiteral("QDialog#levelHeader { background-color: #A1A1A1; }")); // dark gray
#endif

  connect(descEd, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
  connect(okBut, SIGNAL(clicked()), this, SLOT(accept()));
}

QStringList TlevelHeaderWdg::getLevelName() {
  exec();
  QStringList list;
  list << nameEd->text() << descEd->toPlainText();
  return list;
}

void TlevelHeaderWdg::onTextChanged() {
  if (descEd->toPlainText().length() > 120 )
    descEd->setPlainText(descEd->toPlainText().right(120));
}
