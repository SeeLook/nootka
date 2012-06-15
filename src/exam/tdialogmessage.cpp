/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/



#include "tdialogmessage.h"
#include <QLabel>
#include <QPainter>

TdialogMessage::TdialogMessage(const QRect &parentGeo, TQAunit &question, QWidget *parent) :
    QDialog(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
{
    setWindowOpacity(0.8);
	setStyleSheet("background:transparent;");
//     setAttribute(Qt::WA_TranslucentBackground, true);
	
    setGeometry(parentGeo.left() + parentGeo.width() / 2, parentGeo.top() + parentGeo.height() / 2, parentGeo.width() / 4, parentGeo.height() / 4);
    QLabel *mainLab = new QLabel("<br><span style=\"font-size: 30px;\">lets try</span>", this);
    mainLab->setAlignment(Qt::AlignCenter);
	show();
}


void TdialogMessage::paintEvent(QPaintEvent *paintEvent) {
	QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 0, 0, 127));

    painter.drawEllipse(0, 0, width(), height());
}


/*
void MainWindow::on_actionAlways_on_Top_triggered(bool checked)
{
#ifdef Q_OS_WIN
    // #include <windows.h>
    if (checked)
    {
        SetWindowPos(this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    else
    {
        SetWindowPos(this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#else
    Qt::WindowFlags flags = this->windowFlags();
    if (checked)
    {
        this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        this->show();
    }
#endif
}

QPixmap qpm(":/alphamask.png"); 
qpm = qpm.scaled(this->width(),this->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransfor‌​mation);
if (!qpm.isNull()) 
  this->setMask(qpm.mask());
  this->setWindowOpacity(0.9);
*/