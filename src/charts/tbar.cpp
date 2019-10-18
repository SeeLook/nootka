/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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

#include "tbar.h"
#include "tgroupedqaunit.h"
// #include "tquestionpoint.h"
#include <exam/textrans.h>
#include <exam/texam.h>
#include <tcolor.h>

#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qapplication.h>


#define WIDTH (30) // default width of a bar


QString trStyle(QColor c) {
  c.setAlpha(50);
  return QString("style=\"%1\"").arg(Tcolor::bgTag(c));
}


Tbar::Tbar(qreal height, TgroupedQAunit* qaGroup, Tbar::Ekind tipType) :
  QGraphicsObject(),
  m_height(height),
  m_qaGroup(qaGroup)
{
  kind = e_line;
  p_qaPtr.color = qApp->palette().window().color(); //QColor(0, 192, 192);
  m_wrongAt = (qreal)m_qaGroup->mistakes() / (qreal)m_qaGroup->size();
  m_notBadAt = (qreal)m_qaGroup->notBad() / (qreal)m_qaGroup->size();
//     TdropShadowEffect *shadow = new TdropShadowEffect();
//   if (qaGroup) {
//     tipText = QApplication::translate("TstatisticTip", "Statistics") + QLatin1String("<br>");
//     tipText += qaGroup->fullDescription() + QLatin1String("<hr>");
//     tipText += QLatin1String("<table>");
//     if (tipType != e_mistakes)
//         tipText += QLatin1String("<tr><td>") + TexTrans::effectTxt() + QLatin1String(": </td><td style=\"text-align: center;\"> <b>")
//         + QString("%1 %").arg(qaGroup->effectiveness(), 2, 'f', 0, '0') + QLatin1String("</b></td></tr>");
//     tipText += QLatin1String("<tr><td>") + TexTrans::averAnsverTimeTxt() + QLatin1String(": </td><td style=\"text-align: center;\"> <b>")
//             + Texam::formatReactTime(qaGroup->averTime(), true) + QLatin1String("</b></td></tr>");
//     if (tipType != e_mistakes)
//         tipText += QLatin1String("<tr><td>") + QApplication::translate("TanalysDialog", "Questions number")
//                 + QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>").arg(qaGroup->size());
//     if (qaGroup->size() > (qaGroup->mistakes() + qaGroup->notBad())) {
//       tipText += QLatin1String("<tr ") + trStyle(TquestionPoint::goodColor()) + QLatin1String("><td>");
//       tipText += TexTrans::corrAnswersNrTxt() + QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>")
//         .arg(qaGroup->size() - qaGroup->mistakes() - qaGroup->notBad());
//     }
//     if (qaGroup->mistakes())
//       tipText += "<tr " + trStyle(TquestionPoint::wrongColor())  + "><td>" + TexTrans::mistakesNrTxt() +
//               QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>").arg(qaGroup->mistakes());
//     if (qaGroup->notBad())
//       tipText += QLatin1String("<tr ") + trStyle(TquestionPoint::notBadColor()) + QLatin1String("><td>") + TexTrans::halfMistakenTxt() +
//               QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>").arg(qaGroup->notBad());
//     tipText += QLatin1String("</table>");
//   }
}


Tbar::~Tbar()
{}


void Tbar::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

  QRectF rect = boundingRect();
  qreal nextAt = 0.0;
  QColor endColor;
  QLinearGradient grad(0, -rect.height(), 0, 0);
  if (m_wrongAt) {
//     grad.setColorAt(0.0, TquestionPoint::wrongColor());
    nextAt += m_wrongAt;
//     grad.setColorAt(nextAt - 0.01, TquestionPoint::wrongColor());
//     endColor = TquestionPoint::wrongColor();
  }
  if (m_notBadAt) {
//     grad.setColorAt(nextAt, TquestionPoint::notBadColor());
    nextAt += m_notBadAt;
//     grad.setColorAt(nextAt - 0.01, TquestionPoint::notBadColor());
//     endColor = TquestionPoint::notBadColor();
  }
  if (m_qaGroup->mistakes() + m_qaGroup->notBad() < m_qaGroup->size()) {
//     grad.setColorAt(nextAt, TquestionPoint::goodColor());
//     endColor = TquestionPoint::goodColor();
  }
  grad.setColorAt(1.0, endColor);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(grad));
  painter->drawRoundedRect(rect, 1, 1);
}


QRectF Tbar::boundingRect() const {
    QRectF rect(WIDTH / -2, - m_height, WIDTH, m_height); // origin is in the centre on the bottom
    return rect;
}






