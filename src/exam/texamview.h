/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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


#ifndef TEXAMVIEW_H
#define TEXAMVIEW_H

#include <QWidget>
#include <QTime>


class QLCDNumber;
class QLabel;
class QTime;

    /** A @class TexamView represents status of exam.
    * It displays times and numbers of valid/invalid questions.
    */
class TexamView : public QWidget
{
    Q_OBJECT
public:
    explicit TexamView(QWidget *parent = 0);
    void startQuestion();
    void stopQuestion();

signals:

public slots:

private:

    QLCDNumber *m_averTime, /**m_reactTimeLCD,*/ *m_totalTime;
    QLabel *m_reactTimeLab;
    QLabel *m_mistLab, *m_corrLab, *m_effLab;
    QTime m_reactTime;

};

#endif // TEXAMVIEW_H
