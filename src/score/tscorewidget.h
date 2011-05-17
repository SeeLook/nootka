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

#ifndef TSCOREWIDGET_H
#define TSCOREWIDGET_H

#include "tscorewidgetsimple.h"

class QAction;
class QMenu;
class Tnote;
class TkeySignature;

class TscoreWidget : public TscoreWidgetSimple
{
    Q_OBJECT
public:
    explicit TscoreWidget(unsigned char _notesCount =1, QWidget* parent = 0);

    void setEnableEnharmNotes(bool isEnabled);
    void acceptSettings();
    void askQuestion(Tnote note, char realStr = 0);
    void askQuestion(Tnote note, TkeySignature key, char realStr = 0);
    void clearScore();

signals:
    void noteChanged(int index, Tnote note);

public slots:
    void whenNoteWasChanged(int index, Tnote note);

protected:
    void paintEvent(QPaintEvent *event);


};

#endif // TSCOREWIDGET_H
