/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "ttouchstyle.h"
#include <tmtr.h>
#include <QtWidgets/qstylefactory.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>


TtouchStyle::TtouchStyle() :
  QProxyStyle(QStyleFactory::create("Fusion")) // android style is weird
{
  qApp->setStyleSheet(
        QString("QMenu::item { height: %1px; margin: 5px; padding: 4px 10px 4px %2px; min-width: %3px; }").
                      arg(Tmtr::fingerPixels() * 0.7).arg(Tmtr::fingerPixels() * 0.9).arg(Tmtr::longScreenSide() / 5) // height, left/right padding
      + QString("QListWidget::item { min-height: %1px; }").arg(Tmtr::fingerPixels() * 0.75) +
        QString("QSpinBox { min-width: %1px; }").arg(Tmtr::fingerPixels() * 2) +
        QString("QSpinBox::up-button { subcontrol-position: left; width: %1px; height: %1px;}").arg(Tmtr::fingerPixels() * 0.7) +
        QString("QSpinBox::down-button { subcontrol-position: right; width: %1px; height: %1px;}").arg(Tmtr::fingerPixels() * 0.7)
  );
  qApp->setGlobalStrut(QSize(qApp->globalStrut().width(), qMax<int>(qApp->globalStrut().height(), Tmtr::fingerPixels() * 0.7)));
}


int TtouchStyle::styleHint(QStyle::StyleHint hint, const QStyleOption* option,
                           const QWidget* widget, QStyleHintReturn* returnData) const {
  switch (hint) {
  case SH_Menu_Scrollable: // single column, scrollable menu
    return true;
  case SH_Menu_FillScreenWithScroll:
    return false;
  case SH_Menu_FlashTriggeredItem:
    return true;
  case SH_Menu_FadeOutOnHide:
    return true;
  case SH_TabBar_Alignment:
    return Qt::AlignCenter;

  default:
    return QProxyStyle::styleHint(hint, option, widget, returnData);
  }
}


int TtouchStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const {
  switch (metric) {
  case PM_SmallIconSize:
    return qRound(Tmtr::fingerPixels() * 0.7); // small icon size

  case PM_IndicatorHeight: // check box size
  case PM_IndicatorWidth:
    return qRound(Tmtr::fingerPixels() * 0.5);
  case PM_LayoutLeftMargin:
  case PM_LayoutRightMargin:
  case PM_LayoutBottomMargin:
  case PM_LayoutTopMargin:
//   case PM_ButtonMargin:
//   case PM_DefaultChildMargin:
//   case PM_DefaultTopLevelMargin:
    return 4;

  case PM_ExclusiveIndicatorHeight: // radio button ellipse size
  case PM_ExclusiveIndicatorWidth:
    return qRound(Tmtr::fingerPixels() * 0.4);

//  case PM_MenuTearoffHeight:
//    return Tmtr::fingerPixels();

  default:
    return QProxyStyle::pixelMetric(metric, option, widget);
  }


}
