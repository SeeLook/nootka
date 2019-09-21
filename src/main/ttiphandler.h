/***************************************************************************
 *   Copyright (C) 2012-2019 by Tomasz Bojczuk                             *
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
#include <exam/tqatype.h>


#define TIP_POS_NUM (4) /**< Number of possible tip positions depends on question/answer combination */

class Tnote;
class TnoteName;
class QTimer;
class TexamExecutor;
class QTimer;
class Texam;
class TQAunit;
class QQuickItem;


/**
 * This class manages exam/exercise tips (flow information controls)
 */
class TtipHandler : public QObject
{

  friend class TexamExecutor;

  Q_OBJECT

  Q_PROPERTY(QPointF tipPos READ tipPos WRITE setTipPos)
  Q_PROPERTY(QQuickItem* startTip READ startTip WRITE setStartTip)
  Q_PROPERTY(QQuickItem* tryAgainTip READ tryAgainTip WRITE setTryAgainTip)
  Q_PROPERTY(QQuickItem* questionTip READ questionTip WRITE setQuestionTip)
  Q_PROPERTY(QQuickItem* resultTip READ resultTip WRITE setResultTip)
  Q_PROPERTY(QQuickItem* whatNextTip READ whatNextTip WRITE setWhatNextTip)
  Q_PROPERTY(QQuickItem* confirmTip READ confirmTip WRITE setConfirmTip)
  Q_PROPERTY(QQuickItem* certTip READ certTip WRITE setCertTip)

public:

    /**
     * Describes a kind of tip position depended on q/a type - over what widget tip is placed.
     * Number of enumerators has to correspond with @p TIP_POS_NUM definition
     */
  enum EtipPos {
    e_instrumentOver = 0, e_scoreOver = 1, e_nameOver = 2, e_bottomRight
  };
  Q_ENUM(EtipPos)

  TtipHandler(Texam* exam, QObject* parent = nullptr);
  ~TtipHandler() override;

  QPointF tipPos() const { return m_lastTipPos; }
  void setTipPos(const QPointF& p);

  QQuickItem* startTip() { return m_startTip; }
  void setStartTip(QQuickItem* stTip) { m_startTip = stTip; }

  QQuickItem* tryAgainTip() { return m_tryAgainTip; }
  void setTryAgainTip(QQuickItem* tryTip) { m_tryAgainTip = tryTip; }

  QQuickItem* questionTip() { return m_questionTip; }
  void setQuestionTip(QQuickItem* questTip) { m_questionTip = questTip; }

  QQuickItem* resultTip() { return m_resultTip; }
  void setResultTip(QQuickItem* resTip) { m_resultTip = resTip; }

  QQuickItem* whatNextTip() { return m_whatNextTip; }
  void setWhatNextTip(QQuickItem* whatItem) { m_whatNextTip = whatItem; }

  QQuickItem* confirmTip() { return m_confirmTip; }
  void setConfirmTip(QQuickItem* confItem) { m_confirmTip = confItem; }

  QQuickItem* certTip() { return m_certifyTip; }
  void setCertTip(QQuickItem* cItem) { m_certifyTip = cItem; }

      /**
       * Cross platform status message:
       */
//   void setStatusMessage(const QString& text, int duration = 0);

      /**
       * Replaces exam pointer given in constructor to the new one.
       */
  void changeExam(Texam* newExam);

      /**
       * show was question correct text, hides after given time
       */
  void showResultTip(TQAunit *answer, int time = 0);

      /**
       * Text with help on an exam start
       */
  void showStartTip();

   /**
    * Text with what to click after an answer.
    * @p isCorrect - was the question correct
    * @p toCorrection - text how to see corrected answer will be shown.
    */
 void showWhatNextTip(bool isCorrect, bool toCorrection = false);

     /**
      * Text with question context
      */
 void showQuestionTip();

    /**
     * "Try again" text
     */
 void showTryAgainTip(int time);

    /**
     * Tip about confirm an answer appears after given time
     */
 void showConfirmTip(int time = 0);

    /**
     * Status message about how to correct a melody notes.
     */
 void melodyCorrectMessage();

//      /** 'to low' or 'to high' text above pitch view @p pitchDiff is float part of pitch */
//  void outOfTuneTip(float pitchDiff);

//      /** Manages an animation of correcting answer as played sound.
//        * Correct position on the guitar is displayed
//        * and start point on the animation depends on question type.  */
//  void correctToGuitar(TQAtype::Etype& question, int prevTime, TfingerPos& goodPos);

  void clearTips(bool resTipAlso = true);

      /**
       * Returns point size of 'A' letter multiplied by 2.
       */
  int bigFont();

  QString startTipText();

//      /** Paints rectangle around given type of widget to mark where is answer. */
//  const QRect& getRect(TQAtype::Etype kindOf);

//      /** Displays message on main window status label with given note as the detected. */
//  void detectedNoteTip(const Tnote& note);

//  QString detectedText(const QString& txt); /**< Returns bigger @p txt in question color. Used for 'detected'  message. */

  bool hasCertificate() { return m_certifyTip != nullptr; }

//  void clearTryAgainTip();
//  void clearMelodyCorrectMessage();
  void certificateTip();
  void deleteCertTip();

//      /** Message on a status bar about currently performed exercise/exam.
//       * It has to be updated whenever correcting melody message is displayed and deleted.*/
//  void levelStatusMessage();

      /**
       * displays message text: Select any note to play it again.
       */
 void playMelodyAgainMessage();

signals:
  void tipChanged();
  void wantStartTip(const QString& text, const QColor& color, const QPointF& pos);
  void wantQuestionTip(const QString& text, const QPointF& pos);
  void wantConfirmTip(const QString& text, const QColor& color, const QPointF& pos);
  void wantResultTip(const QString& text, const QColor& color);
  void wantTryAgainTip();
  void wantWhatNextTip(const QString& text, const QColor& color, const QPointF& pos);
  void wantCertificate();

//  void buttonClicked(const QString&); /**< This signal is emitted when user click image button (a link) on any tip.*/
  void certificateMagicKeys(); /**< When translator wants to see a certificate preview */
//  void correctingFinished(); /**< Emitted when correction animation finish */

protected:
  bool eventFilter(QObject* obj, QEvent* event) override;
  void showConfirmTipSlot();


//protected slots:
//      /** Calls sizeChanged with delay to allow MainWindow deploy its new geometry. */
//  void sizeChangedDelayed(const QRectF& newRect);
//  void sizeChanged();
//  void correctAnimFinished();
//  void tipMoved(); /**< It is used as a slot to store position of a tip moved by user */
//  void tipStateChanged(); /**< It is used as a slot to store minimization state of a tip. */


private:
//  double                          m_scale;

  Texam                             *m_exam;
  QTimer                            *m_timerToConfirm;
  qreal                              m_prevWidth;
//  TfingerPos                     m_goodPos;
 QPointF                             m_posOfQuestTips[TIP_POS_NUM];
//   bool                               m_minimizedQuestion = false;
  bool                               m_melodyCorrectMessage = false;
  EtipPos                            m_questTipPosType; /**< Kind of question tip position */
  int                                m_iconSize; /**< Icon image size on tips calculated from actual font metrics. */
  QPointF                            m_lastTipPos;
  QQuickItem                        *m_startTip = nullptr;
  QQuickItem                        *m_tryAgainTip = nullptr;
  QQuickItem                        *m_questionTip = nullptr;
  QQuickItem                        *m_resultTip = nullptr;
  QQuickItem                        *m_whatNextTip = nullptr;
  QQuickItem                        *m_confirmTip = nullptr;
  QQuickItem                        *m_certifyTip = nullptr;

private:
  void deleteStartTip();
  void deleteTryAgainTip();
  void deleteQuestionTip();
  void deleteResultTip();
  void deleteWhatNextTip();
  void deleteConfirmTip();
  QPointF getTipPosition(EtipPos tp);
//  void setResultPos();
//  void setWhatNextPos();
//  void setStartTipPos();
//  void setQuestionPos();
//  void setTryAgainPos();
//  void setConfirmPos();
//  void setOutTunePos();
//  void updateRelatedPoint();
 EtipPos determineTipPos();

};

#endif // TCANVAS_H
