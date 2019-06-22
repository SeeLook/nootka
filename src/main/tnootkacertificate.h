/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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


#ifndef TNOOTKACERTIFICATE_H
#define TNOOTKACERTIFICATE_H

#include <QtQuick/qquickpainteditem.h>
#include <QtWidgets/qapplication.h>


class Texam;
class QPainter;
class QGraphicsScene;
class QGraphicsTextItem;
class QGraphicsRectItem;
class QGraphicsPixmapItem;


/**
 * Generates certificate of an exam and paints it into @p QQuickPaintedItem.
 * It uses @p QGraphicsScene to render the certificate,
 * because QML doesn't support pdf export yet.
 */
class TnootkaCertificate : public QQuickPaintedItem
{

  Q_OBJECT

public:
  TnootkaCertificate(QQuickItem* parent = nullptr);
  ~TnootkaCertificate() override;

  enum ErenderState {
    e_noRendered, e_renderInProgress, e_renderFinished
  };
  Q_ENUM(ErenderState)

  void update();
  void paint(QPainter* painter) override;

  void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) override;

  Q_INVOKABLE void save();
  Q_INVOKABLE void stopExam();
  Q_INVOKABLE void continueExam();

private:
      /**
       * Creates @p QGraphicsTextItem instance with given html text.
       */
  QGraphicsTextItem* createCertItem(const QString& htmlText);

      /**
       * Replaces [___] tags with appropriate text values
       */
  QString fillCert(QString entry);

private:
  Texam                     *m_exam;
  QGraphicsRectItem         *m_cert = nullptr;
  QGraphicsTextItem         *m_academyI, *m_dateI, *m_studentI, *m_certHeadI, *m_resultsI, *m_witnesI, *m_boardI, *m_stampI;
  QGraphicsPixmapItem       *m_stampPixmap;
  QGraphicsScene            *m_scene;
  qreal                      m_certW, m_certH;
  QImage                    *m_sceneImage = nullptr;
  ErenderState               m_renderState = e_noRendered;
};

#endif // TNOOTKACERTIFICATE_H
