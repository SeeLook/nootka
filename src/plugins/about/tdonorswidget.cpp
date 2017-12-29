/**************************************************************************
*   Copyright (C) 2016-2017 by Tomasz Bojczuk                             *
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


#include "tdonorswidget.h"
#include <tpath.h>
#include <QtWidgets/QtWidgets>


TdonorsWidget::TdonorsWidget(QWidget* parent) :
  QWidget(parent)
{
  QLabel *headLab = new QLabel(QLatin1String("<h2>")
        + QApplication::translate("TaboutNootka", "People and companies who gave material support for the Nootka project")
        + QLatin1String("</h2><br>"), this);
    headLab->setAlignment(Qt::AlignCenter);
    headLab->setWordWrap(true);
  QString sponText = QLatin1String("<ul>");
    sponText += QLatin1String("<li>Aaron Wolf</li>");
    sponText += QLatin1String("<li>Torsten Philipp</li>");
    sponText += QLatin1String("<li>Vincent Bermel</li>");
    sponText += QLatin1String("<li>Tomasz Matuszewski</li>");
    sponText += QLatin1String("<li>Yves Balhant</li>");
    sponText += QString::fromUtf8("<li>Илья Б.</li>");
    sponText += QLatin1String("</ul>");
    sponText += QLatin1String("<p style=\"text-align: center;\"><b><big>THANK YOU!</b></big>");
  QLabel *sponLab = new QLabel(sponText, this);
  QVBoxLayout *lay = new QVBoxLayout;
    lay->addStretch();
    lay->addWidget(headLab);
    lay->addWidget(sponLab);
    lay->addStretch();
  setLayout(lay);
}


void TdonorsWidget::resizeEvent(QResizeEvent*) {
  m_bgPix = QPixmap(Tpath::img("nootka")).scaled(qMin(width(), 512), qMin(height(), 512), Qt::KeepAspectRatio);
}



void TdonorsWidget::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.drawPixmap((width() - m_bgPix.width()) / 2, (height() - m_bgPix.height()) / 2, m_bgPix);
  QColor bgC = palette().base().color();
  bgC.setAlpha(230);
  p.setBrush(bgC);
  p.setPen(Qt::NoPen);
  p.drawRect(0, 0, width(), height());
}
