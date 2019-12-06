/***************************************************************************
 *   Copyright (C) 2019 by Tomasz Bojczuk                                  *
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

#include "tbarchartdelegate.h"
#include "tchartitem.h"
#include <tglobals.h>
#include <exam/textrans.h>
#include <exam/texam.h>
#include <tcolor.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>


#define HH ((0.8333333333333333 - 0.1))


QString trStyle(QColor c) {
  c.setAlpha(50);
  return QString("style=\"%1\"").arg(Tcolor::bgTag(c));
}


TbarChartDelegate::TbarChartDelegate(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
//   setRenderTarget(QQuickPaintedItem::FramebufferObject);
  setAntialiasing(true);

  connect(qApp, &QGuiApplication::paletteChanged, this, [=]{ update(); });
}


TbarChartDelegate::~TbarChartDelegate()
{
  if (m_barInfo)
    delete m_barInfo;
}


void TbarChartDelegate::setChart(TchartItem* ch) {
  if (!m_chart) {
    m_chart = ch;
    emit examChanged();
    connect(m_chart, &TchartItem::examChanged, this, [=]{
        if (m_chart->ignoreSignalExamChanged())
          return;
        if (m_groupNr > -1) { // reset @p group associated with this delegate
          int tmpNr = m_groupNr;
          m_groupNr = -1;
          setGroupNr(tmpNr);
        }
        emit examChanged();
        update();
    });
  }
}


void TbarChartDelegate::setGroupNr(int gr) {
  if (m_groupNr != gr) {
    m_groupNr = gr;
    if (m_groupNr > -1) {
      if (m_groupNr < m_chart->mainChart()->qCount()) {
          m_qaGroup = m_chart->mainChart()->group(m_groupNr);
          m_wrongAt = static_cast<qreal>(m_qaGroup->mistakes()) / static_cast<qreal>(m_qaGroup->size());
          m_notBadAt = static_cast<qreal>(m_qaGroup->notBad()) / static_cast<qreal>(m_qaGroup->size());
          if (m_barInfo)
            delete m_barInfo;
          m_barInfo = nullptr;
      }
    }
    emit groupNrChanged();
    update();
  }
}


void TbarChartDelegate::setHovered(bool h) {
  if (h != m_hovered) {
    m_hovered = h;
    if (m_hovered) {
        if (!m_barInfo) {
          m_barInfo = new TtipInfo();
          m_barInfo->kind = TtipInfo::e_line;
          if (m_qaGroup) {
            QString& tipText = m_barInfo->tipText;
            tipText = QApplication::translate("TstatisticTip", "Statistics") + QLatin1String("<br>");
            tipText += m_qaGroup->fullDescription() + QLatin1String("<hr>");
            tipText += QLatin1String("<table>");
            if (m_chart->settings()->order != Tchart::e_byMistake)
                tipText += QLatin1String("<tr><td>") + TexTrans::effectTxt() + QLatin1String(": </td><td style=\"text-align: center;\"> <b>")
                + QString("%1 %").arg(m_qaGroup->effectiveness(), 2, 'f', 0, '0') + QLatin1String("</b></td></tr>");
            tipText += QLatin1String("<tr><td>") + TexTrans::averAnsverTimeTxt() + QLatin1String(": </td><td style=\"text-align: center;\"> <b>")
                    + Texam::formatReactTime(m_qaGroup->averTime(), true) + QLatin1String("</b></td></tr>");
            if (m_chart->settings()->order != Tchart::e_byMistake)
                tipText += QLatin1String("<tr><td>") + QApplication::translate("TanalysDialog", "Questions number")
                        + QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>").arg(m_qaGroup->size());
            if (m_qaGroup->size() > (m_qaGroup->mistakes() + m_qaGroup->notBad())) {
              tipText += QLatin1String("<tr ") + trStyle(GLOB->correctColor()) + QLatin1String("><td>");
              tipText += TexTrans::corrAnswersNrTxt() + QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>")
                .arg(m_qaGroup->size() - m_qaGroup->mistakes() - m_qaGroup->notBad());
            }
            if (m_qaGroup->mistakes())
              tipText += "<tr " + trStyle(GLOB->wrongColor())  + "><td>" + TexTrans::mistakesNrTxt() +
                      QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>").arg(m_qaGroup->mistakes());
            if (m_qaGroup->notBad())
              tipText += QLatin1String("<tr ") + trStyle(GLOB->notBadColor()) + QLatin1String("><td>") + TexTrans::halfMistakenTxt() +
                      QString(": </td><td style=\"text-align: center;\"> <b>%1</b></td></tr>").arg(m_qaGroup->notBad());
            tipText += QLatin1String("</table>");
          }
          m_barInfo->setColor(QColor(0, 192, 192));
          m_barInfo->setCursorPos(QPointF(parentItem()->parentItem()->x() + width() * 4.0, parentItem()->height() / 2.0));
        }
        m_chart->tipEntered(m_barInfo);
    } else {
        m_chart->tipExited();
    }
    emit hoveredChanged();
  }
}


qreal TbarChartDelegate::yValue() const {
  return m_chart->settings()->order == Tchart::e_byMistake ? static_cast<qreal>(m_qaGroup->size()) : m_qaGroup->averTime() / 10.0;
}


QString TbarChartDelegate::xText() const {
  return m_groupNr > -1 && m_groupNr < m_chart->mainChart()->qCount() ? m_qaGroup->description() : QString();
}


void TbarChartDelegate::paint(QPainter* painter) {
  painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

  painter->setPen(Qt::NoPen);

  QRectF rect(0.0, 0.0, width(), height());
  qreal nextAt = 0.0;
  QColor endColor;
  QLinearGradient grad(0.0, 0.0, 0.0, rect.height());
  if (m_wrongAt) {
    grad.setColorAt(0.0, GLOB->wrongColor());
    nextAt += m_wrongAt;
    grad.setColorAt(nextAt - 0.01, GLOB->wrongColor());
    endColor = GLOB->wrongColor();
  }
  if (m_notBadAt) {
    grad.setColorAt(nextAt, GLOB->notBadColor());
    nextAt += m_notBadAt;
    grad.setColorAt(nextAt - 0.01, GLOB->notBadColor());
    endColor = GLOB->notBadColor();
  }
  if (m_qaGroup->mistakes() + m_qaGroup->notBad() < m_qaGroup->size()) {
    grad.setColorAt(nextAt, GLOB->correctColor());
    endColor = GLOB->correctColor();
  }
  grad.setColorAt(1.0, endColor);
  painter->setBrush(QBrush(grad));
  painter->drawRoundedRect(rect, width() / 8.0, width() / 8.0);
}

