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
#include "texam.h"
#include "tqaunit.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include <QtGui>

TmainChart::TmainChart(Texam* ex, QWidget* parent): 
  TbaseChart(parent)
{
    exam = ex;
    QVBoxLayout *mainLay = new QVBoxLayout;

//     plot = new QwtPlot(tr(""), this);

    QwtPlotCurve *reactTimeLine = new QwtPlotCurve(tr("reaction time"));
	QwtPlotCurve *averTimeLine = new QwtPlotCurve(tr("average reaction time"));
	reactTimeLine->setPen(QPen(QBrush(Qt::darkBlue), 2));
	averTimeLine->setPen(QPen(QBrush(Qt::darkGreen), 2));

    double x[exam->count()];
    double y[exam->count()];
	double a[exam->count()];
    for (int i = 0; i < exam->count(); i++) {
		x[i] = i;
		y[i] = (double)exam->qusetion(i).time / 10.0;
		a[i] = (double)exam->averageReactonTime() / 10.0;
    }
   reactTimeLine->setSamples(x, y, exam->count());
   averTimeLine->setSamples(x, a, exam->count());

	averTimeLine->attach(plot);
    reactTimeLine->attach(plot);

    plot->replot();

//     setLayout(mainLay);

}
