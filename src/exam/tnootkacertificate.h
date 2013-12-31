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


#ifndef TFINISHTIP_H
#define TFINISHTIP_H
#include <QGraphicsObject>


class QGraphicsView;
class TgraphicsTextTip;
class Texam;



/** QGraphicsTextTip with exam summary like piece of paper. */
class TnootkaCertificate : public QGraphicsObject
{
    Q_OBJECT
    
public:
    TnootkaCertificate(QGraphicsView *view, const QString &path, Texam *exam);
		
		virtual ~TnootkaCertificate();
		
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
    qreal width() { return m_width; } /** width of a certificate rectangle */
    qreal height() { return m_height; } /** height of a certificate rectangle */
    
signals:
				/** Emits what user wants to do:
				 * - @p nextQuest to continue exam and get next question 
				 * - @p stopExam to finish exam	 */
		void userAction(QString actionText);
		
protected:
		void removeHints();
		void createHints();
	
protected slots:
		void saveSlot();
		void linkActivatedSlot(QString link);
    
private:
        /** Creates QGraphicsTextItem instance with given html text. */
    QGraphicsTextItem* createCertItem(const QString& htmlText);
    
private:
    Texam 									*m_exam;
		TgraphicsTextTip 				*m_saveHint, *m_closeHint, *m_nextHint;
		QGraphicsRectItem   		*m_cert;
		QGraphicsRectItem				*m_bgRect;
    QString 								 m_path;
    QGraphicsTextItem       *m_academyI, *m_dateI, *m_studentI, *m_certHeadI, *m_resultsI, *m_witnesI, *m_boardI, *m_stampI;
    QGraphicsPixmapItem     *m_stampPixmap;
    qreal                    m_width, m_height;
		QGraphicsView						*m_view; // QGraphicsView containing those items
};

#endif // TFINISHTIP_H
