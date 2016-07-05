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

#include "tscoremeter.h"
#include "tscorestaff.h"
// #include "graphics/tnotepixmap.h"
#include <tnoofont.h>
#include <music/tmeter.h>
#include <widgets/tpushbutton.h>
#include <QtWidgets/QtWidgets>


#define METRUM_HEIGHT (8.0)


TscoreMeter::TscoreMeter(TscoreScene* scene, TscoreStaff* staff) :
  TscoreItem(scene),
  m_meter(new Tmeter(Tmeter::e_4_4)),
  m_isReadOnly(false),
  m_pianoStaff(false),
  m_width(4.0)
{
  setStaff(staff);
  setParentItem(staff);
  setStatusTip(tr("Time signature"));
}


TscoreMeter::~TscoreMeter()
{
  delete m_meter;
}


void TscoreMeter::setMeter(const Tmeter& meter, bool emitSignal) {
  *m_meter = meter;
  m_upperDigit = TnooFont::digit(m_meter->upper());
  m_lowerDigit = TnooFont::digit(m_meter->lower());
  QFontMetrics fm(TnooFont(8));
  m_width = fm.boundingRect(m_upperDigit).width();
  update();

// set notes grouping
  m_groups.clear();
  if (m_meter->lower() == 4) { // simple grouping: one group for each quarter
    m_groups << 24 << 48; // 2/4 and above
    if (m_meter->meter() > Tmeter::e_2_4)
      m_groups << 72;
    if (m_meter->meter() > Tmeter::e_3_4)
      m_groups << 96;
    if (m_meter->meter() > Tmeter::e_4_4)
      m_groups << 120;
    if (m_meter->meter() > Tmeter::e_5_4)
      m_groups << 144;
    if (m_meter->meter() > Tmeter::e_6_4)
      m_groups << 168;
  } else {
    if (m_meter->meter() == Tmeter::e_3_8)
      m_groups << 36;
    else if (m_meter->meter() == Tmeter::e_5_8)
      m_groups << 36 << 60;
    else if (m_meter->meter() == Tmeter::e_6_8)
      m_groups << 36 << 72;
    else if (m_meter->meter() == Tmeter::e_7_8)
      m_groups << 36 << 60 << 84;
    else if (m_meter->meter() == Tmeter::e_9_8)
      m_groups << 36 << 72 << 108;
    else if (m_meter->meter() == Tmeter::e_12_8)
      m_groups << 36 << 72 << 108 << 144;
  }

  if (emitSignal)
    emit meterChanged();
}


void TscoreMeter::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
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


QRectF TscoreMeter::boundingRect() const {
  return QRectF(0.0, 0.0, m_width, m_pianoStaff ? 22.0 : 8.0);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TscoreMeter::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (isReadOnly())
    QGraphicsItem::mousePressEvent(event);
  else {
    TselectMeter menu(*m_meter);
    Tmeter m = menu.exec(event->screenPos());
    if (m.meter() != Tmeter::e_none && m != *m_meter)
      setMeter(m);
  }
}



//#################################################################################################
//###################              TselectMeter       ############################################
//#################################################################################################
TselectMeter::TselectMeter(const Tmeter& meter, QWidget* parent) :
  QMenu(parent)
{
  m_group = new QButtonGroup(this);
  auto lay = new QGridLayout;
  for (int i = 0 ; i < 12; ++i) {
    Tmeter m(Tmeter::Emeter(qPow(2, i)));
    auto button = new TpushButton(QString(), this);
    int s = qApp->screens()[0]->geometry().width() / (qApp->screens()[0]->physicalSize().width() / 9.0);
    button->setIcon(QIcon(m.pixmap(s)));
    button->setIconSize(QSize(s, s));
      lay->addWidget(button, i / 3, i % 3);
    m_group->addButton(button, qPow(2, i));
    button->setCheckable(true);
    if (meter == m) {
      button->setChecked(true);
      button->click(); // click a button with current meter to initialize the group
    }
  }
  setLayout(lay);
  resize(lay->sizeHint());
  connect(m_group, SIGNAL(buttonClicked(int)), this, SLOT(close()));
}


Tmeter TselectMeter::exec(const QPoint& pos) {
  show(); // necessary since Qt 5.6
  QMenu::exec(pos);
  return Tmeter((Tmeter::Emeter)m_group->checkedId());
}









