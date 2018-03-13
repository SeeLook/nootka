/***************************************************************************
 *   Copyright (C) 2011-2018 by Tomasz Bojczuk                             *
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


TlevelHeaderWdg::TlevelHeaderWdg(const QString& levelName, const QString& levelDesc) :
  QDialog(nullptr)
{
  setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog);
  auto mainLay = new QVBoxLayout;
  auto nameLab = new QLabel(tr("Level name:"), this);
  mainLay->addWidget(nameLab);
  m_nameEd = new QLineEdit(levelName, this);
  m_nameEd->setMaxLength(25);
  m_nameEd->setText(levelName.isEmpty() ? tr("new level") : levelName);
  mainLay->addWidget(m_nameEd);
  auto descLab = new QLabel(tr("Level description:"), this);
  mainLay->addWidget(descLab);
  m_descEd = new QTextEdit(levelDesc, this);
  m_descEd->setFixedHeight(fontMetrics().boundingRect(QStringLiteral("A")).height() * 4);
  m_descEd->setFixedWidth(fontMetrics().boundingRect(QStringLiteral("w")).width() * 30);
  m_descEd->setLineWrapMode(QTextEdit::FixedColumnWidth);
  m_descEd->setLineWrapColumnOrWidth(35);
  mainLay->addWidget(m_descEd);
  m_okBut = new QPushButton(qTR("QDialogButtonBox", "OK"), this);
  mainLay->addWidget(m_okBut, 1, Qt::AlignCenter);

  setLayout(mainLay);

#if defined (Q_OS_ANDROID)
  move((Tmtr::screenWidth() - m_descEd->width()) / 2, Tmtr::fingerPixels() / 2);
  // for better visibility where there is no window frame on mobile change background color
  setObjectName(QStringLiteral("levelHeader"));
  setStyleSheet(QStringLiteral("QDialog#levelHeader { background-color: #A1A1A1; }")); // dark gray
#endif

  connect(m_descEd, &QTextEdit::textChanged, this, &TlevelHeaderWdg::onTextChanged);
  connect(m_okBut, &QPushButton::clicked, this, &TlevelHeaderWdg::accept);
}


QStringList TlevelHeaderWdg::getLevelName() {
  exec();
  QStringList list;
  list << m_nameEd->text() << m_descEd->toPlainText();
  return list;
}


void TlevelHeaderWdg::onTextChanged() {
  if (m_descEd->toPlainText().length() > 120 )
    m_descEd->setPlainText(m_descEd->toPlainText().right(120));
}
