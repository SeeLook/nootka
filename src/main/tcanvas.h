/***************************************************************************
 *   Copyright (C) 2012-2017 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
//#include <QtCore/qpointer.h>
#include <exam/tqatype.h>
//#include <tfingerpos.h>


#define TIP_POS_NUM (4) /**< Number of possible tip positions depends on question/answer combination */

class Tnote;
class TnoteName;
//class TnootkaCertificate;
class QTimer;
class TexamExecutor;
class QTimer;
class Texam;
class TQAunit;


/**
 * This class managing Nootka main View
 * to show notifications (tips) during an exam.
 */
class Tcanvas : public QObject
{

  friend class TexamExecutor;

  Q_OBJECT

  Q_PROPERTY(QPointF tipPos READ tipPos WRITE setTipPos)

public:

      /**
       * Describes a kind of tip position depended on q/a type - over what widget tip is placed.
       * Number of enumerators has to correspond with @p TIP_POS_NUM definition
       */
  enum EtipPos {
    e_instrumentOver = 0, e_scoreOver = 1, e_nameOver = 2, e_bottomRight
  };
  Q_ENUM(EtipPos)

  Tcanvas(Texam* exam, QObject* parent = nullptr);
  ~Tcanvas() override;

  QPointF tipPos() const { return m_lastTipPos; }
  void setTipPos(const QPointF& p);

//    /**
//     * Cross platform status message:
//     * - status bar on desktops (@p TstatusLabel)
//     * - pop up message on screen bottom for mobile (@p TtouchMessage)
//     */
//  void setStatusMessage(const QString& text, int duration = 0);

  void changeExam(Texam* newExam); /**< Replaces exam pointer given in constructor to the new one. */

//  void resultTip(TQAunit *answer, int time = 0); /**< show was question correct text, hides after given time */
  void startTip(); /**< Text with help on an exam start */

//    /**
//     * Text with what to click after an answer.
//     * @p isCorrect - was the question correct
//     * @p toCorrection - text how to see corrected answer will be shown.
//     */
//  void whatNextTip(bool isCorrect, bool toCorrection = false);

     /** Text with question context */
 void questionTip();
//  void tryAgainTip(int time); /**< "Try again" text" */
 void confirmTip(int time = 0); /**< tip about confirm an answer appears after given time */
//  void melodyCorrectMessage(); /**< Status message about how to correct a melody notes. */

//      /** 'to low' or 'to high' text above pitch view @p pitchDiff is float part of pitch */
//  void outOfTuneTip(float pitchDiff);

//      /** Manages an animation of correcting answer as played sound.
//        * Correct position on the guitar is displayed
//        * and start point on the animation depends on question type.  */
//  void correctToGuitar(TQAtype::Etype& question, int prevTime, TfingerPos& goodPos);

  void clearCanvas();

  int bigFont(); /**< Returns point size of 'A' letter multiplied by 2. */

  QString startTipText();


//      /** Paints rectangle around given type of widget to mark where is answer. */
//  const QRect& getRect(TQAtype::Etype kindOf);

//      /** Displays message on main window status label with given note as the detected. */
//  void detectedNoteTip(const Tnote& note);

//  QString detectedText(const QString& txt); /**< Returns bigger @p txt in question color. Used for 'detected'  message. */

//  bool hasCertificate() { return m_certifyTip != nullptr; }

//  void clearTryAgainTip();
 void showConfirmTip();
//  void clearCertificate();
//  void clearMelodyCorrectMessage();
//  void certificateTip(); /**< paper like exam report when finished */

//      /** Message on a status bar about currently performed exercise/exam.
//       * It has to be updated whenever correcting melody message is displayed and deleted.*/
//  void levelStatusMessage();
//  void playMelodyAgainMessage(); /**< displays message text: Select any note to play it again. */

signals:
  void tipChanged();
  void showStartTip(const QString text, const QColor& color, const QPointF& pos);
  void showQuestionTip(const QString text, const QPointF& pos);
  void destroyTips();
//  void buttonClicked(const QString&); /**< This signal is emitted when user click image button (a link) on any tip.*/
//  void certificateMagicKeys(); /**< When translator wants to see a certificate preview */
//  void correctingFinished(); /**< Emitted when correction animation finish */

//protected:
//  virtual bool eventFilter(QObject* obj, QEvent* event);


//protected slots:
//      /** Calls sizeChanged with delay to allow MainWindow deploy its new geometry. */
//  void sizeChangedDelayed(const QRectF& newRect);
//  void sizeChanged();
//  void correctAnimFinished();
//  void tipMoved(); /**< It is used as a slot to store position of a tip moved by user */
//  void tipStateChanged(); /**< It is used as a slot to store minimization state of a tip. */


private:
//  double                          m_scale;

//  TnootkaCertificate             *m_certifyTip;
  Texam                             *m_exam;
  QTimer                            *m_timerToConfirm;
  qreal                              m_prevWidth;
//  TfingerPos                     m_goodPos;
 QPointF                             m_posOfQuestTips[TIP_POS_NUM];
//  bool                           m_minimizedQuestion, m_melodyCorrectMessage;
  EtipPos                            m_questTipPosType; /**< Kind of question tip position */
  int                                m_iconSize; /**< Icon image size on tips calculated from actual font metrics. */
  QPointF                            m_lastTipPos;

private:
  QPointF getTipPosition(EtipPos tp);
//  int getMaxTipHeight(); /**< Calculates maximal tip height depends on free MainWindow widget. */
//  void setPosOfTip(TgraphicsTextTip *tip); /**< Universal method to place given tip above free MainWindow widget.  */
//  void setResultPos();
//  void setWhatNextPos();
//  void setStartTipPos();
//  void setQuestionPos();
//  void setTryAgainPos();
//  void setConfirmPos();
//  void setOutTunePos();
//  void updateRelatedPoint();
//  void createQuestionTip(); /**< Be sure that @p m_exam has already pointed current exam */
//  void fixWidthOverScore(TgraphicsTextTip* tip); /**< Scales tip if its width is bigger than score widget */
 EtipPos determineTipPos();

};

#endif // TCANVAS_H
