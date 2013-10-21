/***************************************************************************
 *   Copyright (C) 2012-2013 by Tomasz Bojczuk                             *
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


#ifndef TCANVAS_H
#define TCANVAS_H

#include <QGraphicsView>
#include "tqatype.h"

class TfinishTip;
class QTimer;
class TtipScene;
class TexamExecutor;
class QParallelAnimationGroup;
class TanimedTextItem;
class QTimer;
class Texam;
class TquestionTip;
class MainWindow;
class TQAunit;
class TgraphicsTextTip;


/** This is view/scene widget laying over centralWidget() 
* to show notifications during an exam.  */
class Tcanvas : public QGraphicsView
{

  friend class TexamExecutor;
  
  Q_OBJECT
  
public:
    Tcanvas(MainWindow *parent);
    virtual ~Tcanvas();
    
    void addTip(TgraphicsTextTip *tip); // add any TgraphicsTextTip object
    void resultTip(TQAunit *answer, int time = 0); // show was question correct text, hides after given time
    void startTip(); // Text with help on an exam start
			/** Text with what to click after an answer.
			 * @p isCorrect - was the question correct
			 * @p toCorrection - text how to see corrected answer will be shown. */
    void whatNextTip(bool isCorrect, bool toCorrection = false);
    void questionTip(Texam *exam); // Text with question context
    void noteTip(int time); // note symbol when sound was detected
    void tryAgainTip(int time); // "Try again" text"
    void confirmTip(int time = 0); // tip about confirm an answer appears after given time
    void finishTip(); // paper like exam report when finished
    
    void clearCanvas();
        /** Returns point size of 'A' letter multiplied by 2. */
    int bigFont();
        /** Returns default font with point size scaled to 'A' letter multiplied by given factor. */
    QFont tipFont(qreal factor = 1);
    QString startTipText();
        /** Paints animated exclamation mark over answering widget. */
    void markAnswer(TQAtype::Etype qType, TQAtype::Etype aType);
        /** Paints rect around given type of widget to mark where is answer. */
    const QRect& getRect(TQAtype::Etype kindOf);
    
public slots:
    void clearResultTip(); // clears tip with results
    void clearTryAgainTip();
    void linkActivatedSlot(QString link);
    void clearConfirmTip();
    void showConfirmTip();
  
signals:
				/** This signal is emitted when user click image button on the some tip.*/
    void buttonClicked(QString name);
      
    
protected:
    bool event(QEvent *event);
    
protected slots:
				/** Calls sizeChanged with delay to allow MainWindow deploy its new geometry. */
		void sizeChangedDelayed(QSize newSize);
    void sizeChanged();
    
    
private:
    MainWindow 										*m_parent;
    QGraphicsScene 								*m_scene;
    double 												m_scale;
    TgraphicsTextTip 							*m_resultTip, *m_whatTip, *m_startTip, *m_tryAgainTip, *m_confirmTip;
    TquestionTip 									*m_questionTip;
    TfinishTip 										*m_finishTip;
    Texam 												*m_exam;
    TanimedTextItem 							*m_flyAnswer, *m_flyNote;
    QParallelAnimationGroup 			*m_animation;
    QTimer 												*m_timerToConfirm;
		int 													m_maxTipWidth;
		bool 													m_guitarFree, m_nameFree, m_scoreFree;
		QSize 												m_newSize;
    
    
private:
			/** Calculates maximal tip height depends on free MainWindow widget. */
		int getMaxTipHeight();
				/** Universal method to place given tip above free MainWindow widget.  */
		void setPosOfTip(TgraphicsTextTip *tip);
    void setPosOfResultTip();
    void setPosOfWhatTip();
    void setPosOfStartTip();
    void setPosOfQuestionTip();
    void setPosOfTryAgainTip();
    void setPosOfConfirmTip();
    void setPosOfFinishTip();
 
};

#endif // TCANVAS_H
