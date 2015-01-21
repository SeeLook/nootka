/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#include "tmenu.h"
#include <QShowEvent>

/*static*/
QWidget* Tmenu::m_statusWidget = 0;




Tmenu::Tmenu(QWidget* parent) :
  QMenu(parent)
{}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void Tmenu::showEvent(QShowEvent* event) {
  if (parentWidget() && m_statusWidget && m_statusWidget->isVisible())
#if defined (Q_OS_WIN)
    move(x(), y() + m_statusWidget->height() + 10);
#else
    move(x(), parentWidget()->y() + 2);
#endif
  QWidget::showEvent(event);
}







