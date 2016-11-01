/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tbgpixmap.h"
#include <music/tinstrument.h>
#include <tpath.h>
#include <QtGui/qpainter.h>


TbgPixmap* TbgPixmap::m_instance = nullptr;


TbgPixmap::TbgPixmap() :
  QPixmap(),
  m_instr(static_cast<int>(e_noInstrument)),
  m_rightHanded(true)
{
  if (m_instance)
    return;

  m_instance = this;
}


TbgPixmap::~TbgPixmap()
{
  m_instance = nullptr;
}


void TbgPixmap::update(const QSize& windowSize, int instr, int guitarHeight, int XIIfretXpos, int fingerboardX, bool rightHanded) {
  if (windowSize != m_windowSize || instr != m_instr || m_rightHanded != rightHanded) {
    Einstrument in = static_cast<Einstrument>(instr);
    if (in == e_noInstrument) {
        QPixmap nullPix;
        swap(nullPix);
    } else {
        QPixmap bodyPix;
        qreal guitH;
        qreal ratio;
        QTransform t;
        if (!rightHanded) {
            t.translate(windowSize.width(), 0.0);
            t.scale(-1.0, 1.0);
        }
        if (in == e_classicalGuitar) {
            guitH = qRound((guitarHeight / 350.0) * 856.0);
            int guitW = windowSize.width() / 2;
            bodyPix = QPixmap(Tpath::img("body")).scaled(guitW, guitH, Qt::IgnoreAspectRatio); // size 800x535
            if (!rightHanded)
              bodyPix = bodyPix.transformed(t);
            int bodyX = rightHanded ? XIIfretXpos + 7 : (windowSize.width() - (XIIfretXpos + 7)) - bodyPix.width();
            m_pos = QPoint(bodyX, windowSize.height() - bodyPix.height());
            m_pickRect = QRect();
        } else {
            if (in == e_bassGuitar)
                bodyPix = QPixmap(Tpath::img("body-bass"));
            else
                bodyPix = QPixmap(Tpath::img("body-electro"));
            guitH = qRound(2.9 * guitarHeight);
            ratio = guitH / bodyPix.height();
            bodyPix = bodyPix.scaled(qRound(bodyPix.width() * ratio), guitH, Qt::KeepAspectRatio);
            if (!rightHanded)
              bodyPix = bodyPix.transformed(t);
            int bodyX = rightHanded ? fingerboardX - 1.449532710280374 * guitarHeight :
                                            (windowSize.width() - (fingerboardX - 1.449532710280374 * guitarHeight) - bodyPix.width());
            m_pos = QPoint(bodyX, windowSize.height() - bodyPix.height());

            qreal pickCoef = 0.002833876221498371 * guitarHeight;
            QPixmap pickUpPix = QPixmap(Tpath::img("pickup")).scaled(291 * pickCoef, 468 * pickCoef, Qt::KeepAspectRatio);
            pickCoef = (guitarHeight * 3.3) / 535;
            int xPic = windowSize.width() * 0.8571428571 + 20 * pickCoef;
            int yPic = (windowSize.height() - guitarHeight) - 30 * pickCoef;
            if (!rightHanded)
                xPic = windowSize.width() - xPic - pickUpPix.width(); // reversed
            m_pickRect = QRect(QPoint(xPic, yPic), pickUpPix.size());
            QPainter painter(&bodyPix);
            painter.drawPixmap(m_pickRect.x() - bodyX, m_pickRect.y() - (windowSize.height() - bodyPix.height()), pickUpPix);
        }
        swap(bodyPix);
    }
    m_windowSize = windowSize;
    m_instr = instr;
    m_rightHanded = rightHanded;
  }
}





