/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tstatementview.h"
#include "tglobals.h"
#include "mainwindow.h"
#include "tqaunit.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>

extern Tglobals *gl;

TstatementView::TstatementView(MainWindow *parent) :
    QGraphicsView(parent)
{
    m_parent = parent;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
//    setStyleSheet("background-color: rgba (255, 255, 255, 200); border-radius: 10px");
    setStyleSheet("background-color: white");
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    m_scene = new QGraphicsScene();
    setScene(m_scene);

}

void TstatementView::answerSumm(TQAunit answer) {
    setGeometry(m_parent->centralWidget()->width()/2, m_parent->centralWidget()->height()/5,
                m_parent->centralWidget()->width()/2, qRound(m_parent->centralWidget()->height() * 0.6));
//    setGeometry(100,100,100,100);
    m_scene->setSceneRect(0, 0, width(), height());

    QString answTxt;
    if (answer.correct()) { // CORRECT
        answTxt = QString("<span style=\"color: %1;\">").arg(gl->EanswerColor.name());
        answTxt += tr("Exelent !!");
    } else { // WRONG
        answTxt = QString("<span style=\"color: %1;\">").arg(gl->EquestionColor.name());
        if (answer.wrongNote())
            answTxt += tr("Wrong note.");
        if (answer.wrongKey())
            answTxt += tr(" Wrong key signature.");
        if (answer.wrongAccid())
            answTxt += tr(" Wrong accidental.");
        if (answer.wrongPos())
            answTxt += tr(" Wrong position.");
        if (answer.wrongOctave())
            answTxt += tr(" Wrong octave.");
    }
    answTxt += "</span>";
    m_stat = new QGraphicsTextItem();
    m_scene->addItem(m_stat);
    m_stat->setHtml(answTxt);

}
