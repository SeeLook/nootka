/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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

#include <QObject>
#include <QPointer>
#include <QGraphicsView>
#include <exam/tqatype.h>
#include <tfingerpos.h>

class TnoteName;
class TcombinedAnim;
class TnootkaCertificate;
class QTimer;
class TtipScene;
class TexamExecutor;
class QTimer;
class Texam;
class TquestionTip;
class MainWindow;
class TQAunit;
class TgraphicsTextTip;


/** 
 * This class managing Nootka main View
 * to show notifications during an exam.  
 */
class Tcanvas : public QObject
{

  friend class TexamExecutor;
  
  Q_OBJECT
  
public:
    Tcanvas(QGraphicsView* view, MainWindow *parent);
    virtual ~Tcanvas();
    
    void addTip(TgraphicsTextTip *tip); // add any TgraphicsTextTip object
    void resultTip(TQAunit *answer, int time = 0); // show was question correct text, hides after given time
    void startTip(); // Text with help on an exam start
		
			/** Text with what to click after an answer.
			 * @p isCorrect - was the question correct
			 * @p toCorrection - text how to see corrected answer will be shown. */
    void whatNextTip(bool isCorrect, bool toCorrection = false);
    void questionTip(Texam *exam); // Text with question context
    void tryAgainTip(int time); // "Try again" text"
    void confirmTip(int time = 0); // tip about confirm an answer appears after given time
    void certificateTip(); // paper like exam report when finished
		void melodyTip(); // How to get hints about corrected melody notes
		
				/** 'to low' or 'to high' text above pitch view @p pitchDiff is float part of pitch */
		void outOfTuneTip(float pitchDiff);		
		
				/** Manages an animation of correcting answer as played sound.
				 * Correct position on the guitar is displayed 
				 * and start point on the animation depends on question type.  */
		void correctToGuitar(TQAtype::Etype& question, int prevTime, TfingerPos& goodPos);
		
    void clearCanvas();
		
        /** Returns point size of 'A' letter multiplied by 2. */
    int bigFont();
		
        /** Returns default font with point size scaled to 'A' letter multiplied by given factor. */
    QFont tipFont(qreal factor = 1);
    QString startTipText();
		
        /** Paints animated exclamation mark over answering widget. */
    void markAnswer(TQAtype::Etype qType, TQAtype::Etype aType);
		
        /** Paints rectangle around given type of widget to mark where is answer. */
    const QRect& getRect(TQAtype::Etype kindOf);
    
public slots:
    void clearResultTip(); // clears tip with results
    void clearTryAgainTip();
    void linkActivatedSlot(QString link);
    void clearConfirmTip();
    void showConfirmTip();
		void clearCertificate();
		void clearCorrection();
		void clearWhatNextTip();
  
signals:
	
				/** This signal is emitted when user click image button on the some tip.*/
    void buttonClicked(QString name);
    void certificateMagicKeys(); // When translator wants to see a certificate preview
      

protected:
//     bool event(QEvent *event);
    
protected slots:
				/** Calls sizeChanged with delay to allow MainWindow deploy its new geometry. */
		void sizeChangedDelayed(const QRectF& newRect);
    void sizeChanged();
		void correctAnimFinished();
    
    
private:
    MainWindow 										*m_window;
		QGraphicsView									*m_view;
    QGraphicsScene 								*m_scene;
    double 												 m_scale;
    QPointer<TgraphicsTextTip>		 m_resultTip, m_whatTip, m_startTip, m_tryAgainTip;
		QPointer<TgraphicsTextTip>		 m_confirmTip, m_outTuneTip, m_melodyTip;
    QPointer<TquestionTip>				 m_questionTip;
    QPointer<TnootkaCertificate>	 m_certifyTip;
    Texam 												*m_exam;
		QPointer<TcombinedAnim>				 m_correctAnim;
    QTimer 												*m_timerToConfirm;
		int 													 m_maxTipWidth;
		bool 													 m_guitarFree, m_nameFree, m_scoreFree;
		QSize 												 m_newSize;
		QGraphicsEllipseItem					*m_flyEllipse;
		TfingerPos										 m_goodPos;
		QColor												 m_correctColor;
		TnoteName											*m_noteName;
		QPoint												 m_relPoint;
    
    
private:
		int getMaxTipHeight(); /** Calculates maximal tip height depends on free MainWindow widget. */
		void setPosOfTip(TgraphicsTextTip *tip); /** Universal method to place given tip above free MainWindow widget.  */
    void setResultPos();
    void setWhatNextPos();
    void setStartTipPos();
    void setQuestionPos();
    void setTryAgainPos();
    void setConfirmPos();
		void setOutTunePos();
		void updateRelatedPoint();
		void setMelodyPos();
 
};

#endif // TCANVAS_H
