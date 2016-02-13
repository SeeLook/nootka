/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtWidgets/qgraphicsview.h>
#include <graphics/tgraphicstexttip.h>
#include <exam/tqatype.h>
#include <tfingerpos.h>

class Tnote;
class TnoteName;
class TcombinedAnim;
class TnootkaCertificate;
class QTimer;
class TtipScene;
class TexamExecutor;
class QTimer;
class Texam;
class TquestionTip;
class TQAunit;



/** HACK
 * Touching a tip doesn't work properly due to mouseMoveEvent spoils emitting clicked() signal.
 * This is workaround for it - to override mousePressEvent() and call clicked() there
 */
class ThackedTouchTip : public TgraphicsTextTip {

  Q_OBJECT

public:
  ThackedTouchTip(const QString& text, QColor bgColor = -1);

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent*) {
    emit clicked();
  }
};



/**
 * This class managing Nootka main View
 * to show notifications (tips) during an exam.
 */
class Tcanvas : public QObject
{

  friend class TexamExecutor;

  Q_OBJECT

public:

	enum EtipPos {
		e_guitarOver = 0, e_scoreOver = 1, e_nameOver = 2
	}; /**< Describes a kind of tip position depended on q/a type - over what widget tip is placed */

	Tcanvas(QGraphicsView* view, Texam* exam);
	virtual ~Tcanvas();

    /** Cross platform status message:
     * - status bar on desktops (@class TstatusLabel)
     * - pop up message on screen bottom for mobile (@class TtouchMessage) */
  void setStatusMessage(const QString& text, int duration = 0);

  void changeExam(Texam* newExam); /**< Replaces exam pointer given in constructor to the new one. */

	void addTip(TgraphicsTextTip *tip); /**< add any TgraphicsTextTip object */
	void resultTip(TQAunit *answer, int time = 0); /**< show was question correct text, hides after given time */
	void startTip(); /**< Text with help on an exam start */

		/** Text with what to click after an answer.
     * @p isCorrect - was the question correct
     * @p toCorrection - text how to see corrected answer will be shown. */
	void whatNextTip(bool isCorrect, bool toCorrection = false);
	void questionTip(); /**< Text with question context */
	void tryAgainTip(int time); /**< "Try again" text" */
	void confirmTip(int time = 0); /**< tip about confirm an answer appears after given time */
  void melodyCorrectMessage(); /**< Status message about how to correct a melody notes. */

			/** 'to low' or 'to high' text above pitch view @p pitchDiff is float part of pitch */
	void outOfTuneTip(float pitchDiff);

			/** Manages an animation of correcting answer as played sound.
				* Correct position on the guitar is displayed 
				* and start point on the animation depends on question type.  */
	void correctToGuitar(TQAtype::Etype& question, int prevTime, TfingerPos& goodPos);

	void clearCanvas();

	int bigFont(); /**< Returns point size of 'A' letter multiplied by 2. */

			/** Returns default font with point size scaled to 'A' letter multiplied by given factor. */
	QFont tipFont(qreal factor = 1);
	QString startTipText();

			/** Paints animated exclamation mark over answering widget. */
	void markAnswer(TQAtype::Etype qType, TQAtype::Etype aType);

			/** Paints rectangle around given type of widget to mark where is answer. */
	const QRect& getRect(TQAtype::Etype kindOf);

			/** Displays message on main window status label with given note as the detected. */
	void detectedNoteTip(const Tnote& note);

  QString detectedText(const QString& txt); /**< Returns bigger @p txt in question color. Used for 'detected'  message. */

public slots:
	void clearResultTip(); // clears tip with results
	void clearTryAgainTip();
	void linkActivatedSlot(const QString& link);
	void clearConfirmTip();
	void showConfirmTip();
	void clearCertificate();
	void clearCorrection();
	void clearWhatNextTip();
  void clearMelodyCorrectMessage();
  void certificateTip(); /**< paper like exam report when finished */

      /** Message on a status bar about currently performed exercise/exam.
       * It has to be updated whenever correcting melody message is displayed and deleted.*/
  void levelStatusMessage();
  void playMelodyAgainMessage(); /**< displays message text: Select any note to play it again. */

signals:
	void buttonClicked(const QString&); /**< This signal is emitted when user click image button (a link) on any tip.*/
	void certificateMagicKeys(); /**< When translator wants to see a certificate preview */
  void correctingFinished(); /**< Emitted when correction animation finish */

protected:
	virtual bool eventFilter(QObject* obj, QEvent* event);


protected slots:
			/** Calls sizeChanged with delay to allow MainWindow deploy its new geometry. */
	void sizeChangedDelayed(const QRectF& newRect);
	void sizeChanged();
	void correctAnimFinished();
  void tipMoved(); /**< It is used as a slot to store position of a tip moved by user */
  void tipStateChanged(); /**< It is used as a slot to store minimization state of a tip. */


private:
	QGraphicsView									*m_view;
	QGraphicsScene 								*m_scene;
	double 												 m_scale;
	QPointer<TgraphicsTextTip>		 m_resultTip, m_whatTip, m_startTip, m_tryAgainTip;
	QPointer<TgraphicsTextTip>		 m_outTuneTip;
	QPointer<TquestionTip>				 m_questionTip;
#if defined (Q_OS_ANDROID)
  QPointer<ThackedTouchTip>      m_nextTip, m_prevTip, m_correctTip, m_confirmTip;
#else
  QPointer<TgraphicsTextTip>     m_confirmTip;
#endif

	QPointer<TnootkaCertificate>	 m_certifyTip;
	Texam 												*m_exam;
	QPointer<TcombinedAnim>				 m_correctAnim;
	QTimer 												*m_timerToConfirm;
	int 													 m_maxTipWidth;
	bool 													 m_guitarFree, m_nameFree, m_scoreFree;
	QSizeF												 m_prevSize;
	QSize 												 m_newSize;
	QGraphicsEllipseItem					*m_flyEllipse;
	TfingerPos										 m_goodPos;
	QColor												 m_correctColor;
	TnoteName											*m_noteName;
	QPoint												 m_relPoint;
	QPointF												 m_posOfQuestTips[3], m_posOfWhatTips[3], m_posOfConfirm;
	bool													 m_minimizedQuestion, m_melodyCorrectMessage;
	EtipPos												 m_tipPos; /**< Kind of tip position */
	int                            m_iconSize; /**< Icon image size on tips calculated from actual font metrics. */

private:
	int getMaxTipHeight(); /**< Calculates maximal tip height depends on free MainWindow widget. */
	void setPosOfTip(TgraphicsTextTip *tip); /**< Universal method to place given tip above free MainWindow widget.  */
	void setResultPos();
	void setWhatNextPos();
	void setStartTipPos();
	void setQuestionPos();
	void setTryAgainPos();
	void setConfirmPos();
	void setOutTunePos();
	void updateRelatedPoint();
	void createQuestionTip(); /**< Be sure that @p m_exam has already pointed current exam */
	void fixWidthOverScore(TgraphicsTextTip* tip); /**< Scales tip if its width is bigger than score widget */

};

#endif // TCANVAS_H
