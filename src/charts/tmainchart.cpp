/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#include "tmainchart.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include <QtGui>

TmainChart::TmainChart(QWidget *parent) :
    QWidget(parent)
{

    QVBoxLayout *mainLay = new QVBoxLayout;

    m_chart = new QwtPlot(tr("chart"), this);

    QwtPlotCurve *reactTimeLine = new QwtPlotCurve(tr("reaction time"));
//    QwtPlotCurve *averTimeLine = new QwtPlotCurve(tr("average reaction time"));

//    QwtSeriesData data;
//    data << QPointF(1, 13) << QPointF(2, 3);

//    reactTimeLine->setData(QPointF(1, 13));
//    reactTimeLine->setData(QPointF(2, 3));
//    reactTimeLine->setData(data);
    double x[2], y[2];
    x[0] = 1; x[1] = 2;
    y[0] = 13; y[2] = 5;
    reactTimeLine->setSamples(x, y, 2);

//    averTimeLine->setData(QPointF(1, 8));
//    averTimeLine->setData(QPointF(2, 16));

    reactTimeLine->attach(m_chart);
//    averTimeLine->attach(m_chart);

    m_chart->replot();

    setLayout(mainLay);

}
