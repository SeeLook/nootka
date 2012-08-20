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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TPROGRESSWIDGET_H
#define TPROGRESSWIDGET_H

#include <QWidget>

class QProgressBar;
class QLabel;

class TprogressWidget : public QWidget
{

public:
    TprogressWidget(QWidget *parent = 0);
    virtual ~TprogressWidget();
    
    static QString progressExamTxt() { return tr("Progress of the exam"); }
    
    void activate(int answers, int total, int penaltys);
    void progress(int total, int penaltys);
    void terminate();
    int total() { return m_totalNr; }
    
    void resize(int fontSize);
    
protected:
    QString zeroLabTxt();
    
private:
    int m_answersNr, m_totalNr;
    QLabel *m_answLab, *m_totalLab;
    QProgressBar *m_bar;
    
};

#endif // TPROGRESSWIDGET_H
