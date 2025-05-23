/***************************************************************************
 *   Copyright (C) 2019-2021 by Tomasz Bojczuk                             *
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

#include "tpiechartitem.h"

#include <QtGui/qguiapplication.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <tglobals.h>

TpieChartItem::TpieChartItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setAntialiasing(true);
}

void TpieChartItem::paint(QPainter *painter)
{
    if (m_valueList.isEmpty()) // nothing to paint
        return;

    int lastPie = 1440; // 1440 = 90 deg * 16 (see drawPie doc if doubts)
    qreal r = painter->viewport().height() * 0.4;
    auto f = painter->font();
    f.setPointSize(painter->viewport().height() / 18);
    f.setBold(true);
    painter->setFont(f);
    painter->setBrush(m_colors.isEmpty() ? Qt::red : QColor(m_colors.first()));
    QFontMetrics fm(f);
    int k;
    for (int v = 0; v < m_valueList.size(); ++v) {
        k = m_valueList[v];
        if (k) {
            painter->setPen(Qt::NoPen);
            int pieSpan = qRound((static_cast<qreal>(k) / m_sum) * 5760.0);
            painter->drawPie(painter->viewport(), lastPie, pieSpan);
            if (pieSpan > 159) { // span width at least 10 deg
                auto nr = QString::number(k);
                QSizeF tOff(fm.tightBoundingRect(nr).size());
                qreal angle = qDegreesToRadians(360.0 - static_cast<double>(lastPie + pieSpan / 2) / 16.0);
                qreal xText = r * qCos(angle) + painter->viewport().width() / 2.0;
                qreal yText = r * qSin(angle) + painter->viewport().height() / 2.0;
                painter->setPen(Qt::white);
                painter->drawText(qRound(xText - tOff.width() / 2.0), qRound(yText + tOff.height() / 2.0), nr);
            }
            lastPie += pieSpan;
        }
        if (v < m_colors.size() - 1)
            painter->setBrush(QColor(m_colors[v + 1]));
        else
            painter->setBrush(painter->brush().color().darker(110));
    }
}

void TpieChartItem::setValues(QList<int> valList)
{
    m_valueList = valList;
    m_sum = 0.0;
    for (int k : m_valueList)
        m_sum += static_cast<qreal>(k);
    emit valuesChanged();
    update();
}

void TpieChartItem::setColors(const QStringList &cl)
{
    m_colors = cl;
    emit colorsChanged();
    update();
}
