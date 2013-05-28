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
 

#ifndef TSIMPLESCORE_H
#define TSIMPLESCORE_H

#include <QWidget>

class QGraphicsView;
class TscoreControl;
class TscoreStaff;
class TscoreScene;


/** This class implements score.  */
class TsimpleScore : public QWidget
{
  Q_OBJECT
  
public:
    TsimpleScore(QWidget *parent = 0);
    ~TsimpleScore();

				/** Sets TsimpleScore to piano staff view.
				 * Clefs are unchangeable. */
		void setPianoStaff(bool isPiano);
		bool isPianoStaff() { return (bool)m_lowerStaff; }
		
		void setEnableKeySign(bool isEnabled);
		
signals:
    void statusTip(QString);
		
protected:
		QGraphicsView* score() { return m_score; }
		TscoreScene* scene() { return m_scene; }
		TscoreStaff* staff() { return m_staff; }
    
protected:
    virtual int heightForWidth(int w) const;
    
protected slots:
    void statusTipChanged(QString status) { emit statusTip(status); }
    void resizeEvent(QResizeEvent* event);
				/** This slot ties key signatutes in two staves. */
		void keyChangedInPianoStaff();
  
private:
    TscoreScene     *m_scene;
    TscoreStaff     *m_staff, *m_lowerStaff;
    TscoreControl   *m_scoreControl;
    QGraphicsView   *m_score;
		
    
  
};

#endif // TSIMPLESCORE_H
