/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "ttouchstyle.h"
#include <tmtr.h>
#include <QStyleFactory>
#include <QDebug>

TtouchStyle::TtouchStyle() :
  QProxyStyle(QStyleFactory::create("Fusion"))
{
  qApp->setStyleSheet(
        QString("QMenu::item { height: %1px; width: %2px; margin: 10px; padding: 2px %1px 2px %1px}").
                      arg(Tmtr::fingerPixels() * 0.7).arg(Tmtr::screenWidth() / 5) +
        QString("QListWidget::item { height: %1px; }").arg(Tmtr::fingerPixels() * 0.7)
        );
}

int TtouchStyle::styleHint(QStyle::StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const {
  if (hint == QStyle::SH_Menu_Scrollable)
    return 1;
//  if (hint == QStyle::SH_Menu_FillScreenWithScroll)
//    return 1;
  else
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}


int TtouchStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const {
  if (metric == PM_SmallIconSize)
    return Tmtr::fingerPixels() * 0.7;
  else if (metric == PM_IndicatorHeight) // check box square height
    return Tmtr::fingerPixels() * 0.5;
  else if (metric == PM_IndicatorWidth) // check box square width
    return Tmtr::fingerPixels() * 0.5;
  else if (metric == PM_ExclusiveIndicatorHeight) // radio ellipse height
    return Tmtr::fingerPixels() * 0.4;
  else if (metric == PM_ExclusiveIndicatorWidth) // radio ellipse width
    return Tmtr::fingerPixels() * 0.4;
  else if (metric == PM_MenuTearoffHeight)
    return Tmtr::fingerPixels();
  else
    return QProxyStyle::pixelMetric(metric, option, widget);
}
