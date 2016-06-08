/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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


#ifndef TFINISHTIP_H
#define TFINISHTIP_H

#include <graphics/tgraphicstexttip.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>


class QGraphicsView;
class TgraphicsTextTip;
class Texam;

/** HACK
 * Touching a tip doesn't work properly due to mouseMoveEvent spoils emitting clicked() signal.
 * This is workaround for it - to override mousePressEvent() and call clicked() there
 */
class ThackedTouchTip : public TgraphicsTextTip {

  Q_OBJECT

public:
  ThackedTouchTip(const QString& text, QColor bgColor = -1) :
    TgraphicsTextTip(text, bgColor) {
      setBaseColor(qApp->palette().text().color());
      setDefaultTextColor(qApp->palette().base().color());
    }

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent*) {
    emit clicked();
  }
};



/**
 * QGraphicsTextTip with exam summary like piece of paper.
 */
class TnootkaCertificate : public QGraphicsObject
{

  Q_OBJECT

public:
  TnootkaCertificate(QGraphicsView* view, Texam* exam);

  virtual ~TnootkaCertificate();

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

      /** width of a certificate rectangle */
  qreal width() { return m_width; }

      /** height of a certificate rectangle */
  qreal height() { return m_height; }

signals:
      /** Emits what user wants to do:
        * - @p nextQuest to continue exam and get next question
        * - @p stopExam to finish exam   */
  void userAction(const QString&);

protected:
  void removeHints();
  void createHints();

protected slots:
  void saveSlot();
  void hintClicked();

private:
      /** Creates QGraphicsTextItem instance with given html text. */
  QGraphicsTextItem* createCertItem(const QString& htmlText);

      /** Replaces [___] tags with appropriate text values */
  QString fillCert(QString entry);

private:
  Texam                     *m_exam;
#if defined (Q_OS_ANDROID)
  ThackedTouchTip           *m_saveHint, *m_closeHint, *m_nextHint;
#else
  TgraphicsTextTip          *m_saveHint, *m_closeHint, *m_nextHint;
#endif
  QGraphicsRectItem         *m_cert;
  QGraphicsPixmapItem       *m_bgRect;
  QGraphicsTextItem         *m_academyI, *m_dateI, *m_studentI, *m_certHeadI, *m_resultsI, *m_witnesI, *m_boardI, *m_stampI;
  QGraphicsPixmapItem       *m_stampPixmap;
  qreal                      m_width, m_height;
  QGraphicsView             *m_view; // QGraphicsView containing those items
};

#endif // TFINISHTIP_H
