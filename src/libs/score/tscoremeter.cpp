/***************************************************************************
 *   Copyright (C) 2016-2017 by Tomasz Bojczuk                             *
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
// #include <widgets/tpushbutton.h>
#include <QtWidgets/QtWidgets>


#define METRUM_HEIGHT (8.0)


TscoreMeter::TscoreMeter(TscoreScene* scene, TscoreStaff* staff) :
  TscoreItem(scene),
  m_meter(new Tmeter(Tmeter::Meter_4_4)),
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
    if (m_meter->meter() > Tmeter::Meter_2_4)
      m_groups << 72;
    if (m_meter->meter() > Tmeter::Meter_3_4)
      m_groups << 96;
    if (m_meter->meter() > Tmeter::Meter_4_4)
      m_groups << 120;
    if (m_meter->meter() > Tmeter::Meter_5_4)
      m_groups << 144;
    if (m_meter->meter() > Tmeter::Meter_6_4)
      m_groups << 168;
  } else {
    if (m_meter->meter() == Tmeter::Meter_3_8)
      m_groups << 36;
    else if (m_meter->meter() == Tmeter::Meter_5_8)
      m_groups << 36 << 60;
    else if (m_meter->meter() == Tmeter::Meter_6_8)
      m_groups << 36 << 72;
    else if (m_meter->meter() == Tmeter::Meter_7_8)
      m_groups << 36 << 60 << 84;
    else if (m_meter->meter() == Tmeter::Meter_9_8)
      m_groups << 36 << 72 << 108;
    else if (m_meter->meter() == Tmeter::Meter_12_8)
      m_groups << 36 << 72 << 108 << 144;
  }

  updateOptGap();

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

void TscoreMeter::updateOptGap() {
  if (m_meter->lower() == 4) {
      m_optGap = m_meter->upper() * 2.0; // simple multiply number of quarters by gap for quarter (see TscoreNote)
  } else {
      if (m_meter->meter() == Tmeter::Meter_3_8)
        m_optGap = 2.5; // quarter with dot
      else if (m_meter->meter() == Tmeter::Meter_5_8)
        m_optGap = 2.5 + 2.0; // quarter with dot and quarter
      if (m_meter->meter() == Tmeter::Meter_6_8)
        m_optGap = 2.0 * 2.5; // two quarters with dot
      if (m_meter->meter() == Tmeter::Meter_7_8)
        m_optGap = 2.5 + 2.0 * 2.0; // quarter with dot and two quarters
      if (m_meter->meter() == Tmeter::Meter_9_8)
        m_optGap = 3.0 * 2.5; // three quarters with dot
      if (m_meter->meter() == Tmeter::Meter_12_8)
        m_optGap = 4.0 * 2.5; // four quarters with dot
  }
}










