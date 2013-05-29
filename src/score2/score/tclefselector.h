/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#ifndef TCLEFSELECTOR_H
#define TCLEFSELECTOR_H

#include <QGraphicsTextItem>
#include <tclef.h>

class TscoreScene;

/** This class implements QGraphicsObject which appeaars  when usec cliced on clef.
 * User gets possibility to select a cleff or piano staff. */
// class TclefSelector : public TscoreItem
class TclefSelector : public QGraphicsTextItem
{
    Q_OBJECT

public:
//     TclefSelector(TscoreScene *scene);
    TclefSelector();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
//     virtual QRectF boundingRect();

protected slots:
    void linkActivatedSlot(QString link);

private:
// 		QGraphicsTextItem *m_G, *m_G8;
    /** Returns html string wrapped with nootka font  */
    QString clefToHtml(Tclef clef);

// 		void createEntry(QGraphicsTextItem* textItem, Tclef clef);

};

#endif // TCLEFSELECTOR_H
