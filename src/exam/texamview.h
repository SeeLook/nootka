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
#include <QTimer>


//class QLCDNumber;
class QLabel;
class QTime;
class QTimer;

    /** A @class TexamView represents status of exam.
    * It displays times and numbers of valid/invalid questions.
    */
class TexamView : public QWidget
{
    Q_OBJECT
public:
    explicit TexamView(QWidget *parent = 0);

    void startExam( int passTimeInSec = 0, int questNumber = 0,int averTime = 0, int mistakes = 0);
    void questionStart();
    quint16 questionStop();
    void setAnswer(bool wasCorrect);
    void setFontSize(int s);

    void stopExam() { m_timer->stop(); }
        /** This method returns rounded average time. It is only for exam preview.*/
    quint16 getAverageTime() { return (quint16)qRound(m_averTime); }
//     quint32 getTotalTime() {return m_totElapsedTime + quint32(m_totalTime.hour() * 3600 +
//                                    m_totalTime.minute() *60 + m_totalTime.second()); }
    quint32 getTotalTime() {return m_totElapsedTime + quint32(m_totalTime.elapsed() / 1000); }
    quint16 getMistakesNumber() {return (quint16)m_mistakes; }
    void clearResults();

signals:

public slots:

private:
    bool m_showReact;

//    QLCDNumber *m_averTime, /**m_reactTimeLCD,*/ *m_totalTime;
    QLabel *m_reactTimeLab, *m_averTimeLab, *m_totalTimeLab;
    QLabel *m_mistLab, *m_corrLab, *m_effLab;
    QTime m_reactTime;
    int m_questNr, m_mistakes, m_totElapsedTime;
    qreal m_averTime;
    QTimer *m_timer;
    QTime m_totalTime;

private slots:
    void countTime();

};

#endif // TEXAMVIEW_H
