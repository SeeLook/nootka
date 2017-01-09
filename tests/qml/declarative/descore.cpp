/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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


#include "descore.h"
#include "denote.h"

#include <tscorescene.h>
#include <tscorestaff.h>
#include <tscoreclef.h>
#include <music/tmeter.h>

#include <QtQuick/QSGNode>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtGui/QPainter>

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)
#define CHECKTIME(x)  \
    QElapsedTimer CONCAT(sb_, __LINE__); \
    CONCAT(sb_, __LINE__).start(); \
    x \
    qDebug() << __FUNCTION__ << ":" << __LINE__ << " Elapsed time: " <<  CONCAT(sb_, __LINE__).nsecsElapsed() / 1000 << " μs.";


static int staffNum = 0;

DeScore::DeScore(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
//   setFlag(ItemHasContents, true);
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
  setAntialiasing(true);

  m_sceneImage = nullptr;

  m_scene = new TscoreScene;
  m_staff = new TscoreStaff(m_scene, 0);
  m_staff->setStafNumber(0);
  m_staff->setMeter(Tmeter(Tmeter::e_2_4));
  m_staff->setDisabled(true);

  setAcceptHoverEvents(true);

  staffNum++;
  m_staffNr = staffNum;
}


DeScore::~DeScore()
{
  delete m_scene;
}


DeScore::Eclef DeScore::clef() {
  return static_cast<Eclef>(m_staff->scoreClef()->clef().type());
}


void DeScore::setClef(DeScore::Eclef c) {
  qDebug() << "[DeScore" << m_staffNr << "] set clef" << c;
  m_staff->onClefChanged(Tclef(static_cast<Tclef::Etype>(c)));
//   m_staff->scoreClef()->setClef(Tclef(static_cast<Tclef::Etype>(c)));
  update();
  emit clefChanged();
}


char DeScore::keySignature() {
  return 0;
}


void DeScore::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (boundingRect().width() > 0 && newGeometry != oldGeometry) {
    qDebug() << "[DeScore" << m_staffNr << "] resize" << oldGeometry << newGeometry << boundingRect();
    m_scene->setSceneRect(0.0, 0.0, boundingRect().width(), boundingRect().height());
    m_staff->setScale(newGeometry.height() / m_staff->height());
    m_staff->setViewWidth((newGeometry.width() - 25) / m_staff->scale());
    m_staff->setPos((m_staff->isPianoStaff() ? 2.1 : 1.0) * m_staff->scale(), 0.0);
    update();
  }
  QQuickItem::geometryChanged(newGeometry, oldGeometry);
}


// QSGNode* DeScore::updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData*) {
//   qDebug() << "[DeScore" << m_staffNr << "] node" << boundingRect();
//   QSGSimpleTextureNode *node = static_cast<QSGSimpleTextureNode*>(oldNode);
//   if (!node)
//     node = new QSGSimpleTextureNode();
// 
//   renderScene();
//   auto texture = window()->createTextureFromImage(*m_sceneImage);
//   node->setTexture(texture);
//   node->setOwnsTexture(true);
// 
//   node->setRect(boundingRect());
//   return node;
// }

void DeScore::paint(QPainter* painter) {
CHECKTIME(
//   qDebug() << "[DeScore" << m_staffNr << "] render" << boundingRect();
//   if (m_sceneImage)
//     delete m_sceneImage;
//   m_sceneImage = new QImage(boundingRect().width(), boundingRect().height(), QImage::Format_ARGB32);
//   m_sceneImage->fill(Qt::transparent);

//   QPainter p(m_sceneImage);
  if (!painter->paintEngine())
    return;
//   painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  m_scene->render(painter, boundingRect(), boundingRect());
//   update();
)
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void DeScore::renderScene() {
CHECKTIME(
//   qDebug() << "[DeScore" << m_staffNr << "] render" << boundingRect();
  if (m_sceneImage)
    delete m_sceneImage;
  m_sceneImage = new QImage(boundingRect().width(), boundingRect().height(), QImage::Format_ARGB32);
  m_sceneImage->fill(Qt::transparent);

  QPainter p(m_sceneImage);
  if (!p.paintEngine())
    return;
  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  m_scene->render(&p, boundingRect(), boundingRect());
//   update();
)
}


void DeScore::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value) {
  if (change == QQuickItem::ItemChildAddedChange) {
    auto noteSegment = qobject_cast<DeNote*>(value.item);
    if (noteSegment) {
      qDebug() << "[DeScore" << m_staffNr << "] added note Segment" << (int)noteSegment->pitch() << (int)noteSegment->octave();
      auto ns = m_staff->insertNote(m_staff->count(), *noteSegment->note(), true);
      noteSegment->setNoteSegment(ns);
//       ns->showNoteName(m_scene->nameColor());
      update();
    }
  }
  QQuickItem::itemChange(change, value);
}


void DeScore::hoverMoveEvent(QHoverEvent* event) {
//   auto it = m_scene->itemAt(event->posF(), QTransform());
//   qDebug() << "[DeScore" << m_staffNr << "] item at" << event->posF() << it;
}







