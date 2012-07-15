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



#ifndef TDIALOGMESSAGE_H
#define TDIALOGMESSAGE_H

#include <QWidget>
#include "tnote.h"

class TkeySignature;
class Texam;
class MainWindow;
class TQAunit;
class TexamLevel;
class QLabel;


/** This class reimlements QWidget and show window over main appication window.
 * It looks like widget over widgets. 
 */
class TdialogMessage : public QWidget
{
   Q_OBJECT
public:
        /** Constructs dialog with question content. itself calculates position over Nootka window */
    explicit TdialogMessage(Texam *exam, MainWindow *parent, Tnote::EnameStyle style);
    static QString getTextHowAccid(Tnote::Eacidentals accid);
        /** Returns key signature name depends on minor/major */
    static QString getKeyText(TkeySignature &key);
        /** Returns translated text on (strNr) string in Nootka font. */
    static QString onStringTxt(quint8 strNr);
        /** Returns html-formated question text. */
    QString getQuestion(TQAunit &question, int questNr, TexamLevel *level, Tnote::EnameStyle style);
    QString getNiceNoteName(Tnote note);
    
public slots:
    void mainWindowMoved(QPoint vector);
    void mainWindowMaximised();
    void mainWindowMinimised();
    void mainWindowResized(QSize parentSize);
	
protected:
    virtual void paintEvent(QPaintEvent *);
    
    void setPosAndSize(QSize &size);
    
private:
    QLabel *m_mainLab;
    QPoint m_pos;
    QSize m_size;
    bool m_scoreFree, m_nameFree, m_guitarFree; // Indicate when widgets show question or answer
    MainWindow *m_parent;
};

#endif // TDIALOGMESSAGE_H
