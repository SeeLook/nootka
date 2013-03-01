/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#ifndef TUPDATEPROCESS_H
#define TUPDATEPROCESS_H

#include <QObject>

class QTimer;
class QProcess;

class TupdateProcess : public QObject
{
  
  Q_OBJECT
  
public:
    TupdateProcess(bool respectRules, QObject *parent = 0);
    virtual ~TupdateProcess();
    
    void start();
    bool isPossible() { return m_isPossible; }
    
    
protected slots:
  void timeOut();
  
private:
  bool m_respectRules, m_isPossible;
  QProcess *m_process;
  QTimer *m_timer;
  QString m_exec;
  
};

#endif // TUPDATEPROCESS_H
