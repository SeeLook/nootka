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


#ifndef TCANVAS_H
#define TCANVAS_H

#include <QGraphicsView>
#include "tqatype.h"

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

  Q_OBJECT
  
public:
    Tcanvas(MainWindow *parent);
    virtual ~Tcanvas();
    
    void addTip(TgraphicsTextTip *tip); // add any TgraphicsTextTip object
    void resultTip(TQAunit *answer, int time = 0); // show was question correct text, hides after given time
    void startTip(); // Text with help on an exam start
    void whatNextTip(bool isCorrect, bool onRight = true); // Text with what to click after an answer
    void questionTip(Texam *exam); // Text with question context
    void noteTip(int time); // note symbol when sound was detected
    void tryAgainTip(int time); // "Try again" text"
    
    void clearCanvas();
        /** Returns point size of 'A' letter multipled by 2. */
    int bigFont();
        /** Returns default font with point size scaled to 'A' letter multipled by given factor. */
    QFont tipFont(qreal factor = 1);
    QString startTipText();
        /** Paints animated exclamation mark over answering widged. */
    void markAnswer(TQAtype::Etype qType, TQAtype::Etype aType);
        /** Paints rect around given type of widget to mark where is answer. */
    const QRect& getRect(TQAtype::Etype kindOf);
    
public slots:
    void clearResultTip(); // cleanes tip with results
    void clearNoteTip();
    void clearTryAgainTip();
    
protected slots:
    void sizeChanged(QSize newSize);
    
private:
    MainWindow *m_parent;
    QGraphicsScene *m_scene;
    double m_scale;
    TgraphicsTextTip *m_resultTip, *m_whatTip, *m_startTip, *m_tryAgainTip;
    TquestionTip *m_questionTip;
    Texam *m_exam;
    QTimer *m_noteTimer;
    TanimedTextItem *m_flyAnswer;
    QParallelAnimationGroup *m_animation;
    
    
private:
    void setPosOfResultTip();
    void setPosOfWhatTip();
    void setPosOfStartTip();
    void setPosOfQuestionTip();
    void setPosOfTryAgainTip();
 
};

#endif // TCANVAS_H
