/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "tscoremetrum.h"
#include "tscorestaff.h"
// #include "graphics/tnotepixmap.h"
#include <tnoofont.h>
#include <widgets/tpushbutton.h>
#include <QtWidgets/QtWidgets>


#define METRUM_HEIGHT (8.0)


TscoreMetrum::TscoreMetrum(TscoreScene* scene, TscoreStaff* staff) :
  TscoreItem(scene),
  m_metrum(Tmetrum(Tmetrum::e_4_4)),
  m_isReadOnly(false),
  m_pianoStaff(false),
  m_width(4.0)
{
  setStaff(staff);
  setParentItem(staff);
  setStatusTip(tr("Time signature"));
}


void TscoreMetrum::setMetrum(const Tmetrum& metrum) {
  m_metrum = metrum;
  m_upperDigit = TnooFont::digit(m_metrum.upper());
  m_lowerDigit = TnooFont::digit(m_metrum.lower());
  QFontMetrics fm(TnooFont(8));
  m_width = fm.boundingRect(m_upperDigit).width();
  update();
  emit metrumChanged();
}


void TscoreMetrum::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
//   paintBackground(painter, Qt::yellow);
  painter->setPen(qApp->palette().text().color());
  painter->setFont(TnooFont(8));
  painter->drawText(0, 0, m_width, 4.0, Qt::AlignCenter, m_upperDigit);
  painter->drawText(0, 4, m_width, 4.0, Qt::AlignCenter, m_lowerDigit);
  if (m_pianoStaff) {
    painter->drawText(0, 14, m_width, 4.0, Qt::AlignCenter, m_upperDigit);
    painter->drawText(0, 18, m_width, 4.0, Qt::AlignCenter, m_lowerDigit);
  }
}


QRectF TscoreMetrum::boundingRect() const {
  return QRectF(0.0, 0.0, m_width, m_pianoStaff ? 22.0 : 8.0);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TscoreMetrum::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (isReadOnly())
    QGraphicsItem::mousePressEvent(event);
  else {
    TselectMetrum menu(m_metrum);
    Tmetrum m = menu.exec(event->screenPos());
    if (m.meter() != Tmetrum::e_none && m != m_metrum)
      setMetrum(m);
  }
}



//#################################################################################################
//###################              TselectMetrum       ############################################
//#################################################################################################
TselectMetrum::TselectMetrum(const Tmetrum& metrum, QWidget* parent) :
  QMenu(parent)
{
  m_group = new QButtonGroup(this);
  auto lay = new QGridLayout;
  for (int i = 0 ; i < 12; ++i) {
    Tmetrum m(Tmetrum::Emeter(qPow(2, i)));
    auto button = new TpushButton(QString(), this);
    int s = qApp->screens()[0]->geometry().width() / (qApp->screens()[0]->physicalSize().width() / 9.0);
    button->setIcon(QIcon(m.pixmap(s)));
    button->setIconSize(QSize(s, s));
      lay->addWidget(button, i / 3, i % 3);
    m_group->addButton(button, qPow(2, i));
    button->setCheckable(true);
    if (metrum == m)
      button->setChecked(true);
  }
  setLayout(lay);
  resize(lay->sizeHint());
  connect(m_group, SIGNAL(buttonClicked(int)), this, SLOT(close()));
}


Tmetrum TselectMetrum::exec(const QPoint& pos) {
  QMenu::exec(pos);
  return Tmetrum((Tmetrum::Emeter)m_group->checkedId());
}









