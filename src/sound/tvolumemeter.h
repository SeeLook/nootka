/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

#ifndef TVOLUMEMETER_H
#define TVOLUMEMETER_H

#include <QWidget>


class TvolumeMeter : public QWidget
{
  Q_OBJECT
  
public:
  
  explicit TvolumeMeter(QWidget *parent = 0);
  virtual ~TvolumeMeter();
  
  void setVolume(qreal vol);

protected:
  void paintEvent(QPaintEvent* );

private:
    qreal m_volume;
    QPixmap m_pixmap;
	QLinearGradient m_grad;
	
};

#endif // TVOLUMEMETER_H
