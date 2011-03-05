/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TSCOREWIDGET_H
#define TSCOREWIDGET_H

#include "tscorewidgetsimple.h"

class QAction;
class QMenu;
class Tnote;

class TscoreWidget : public TscoreWidgetSimple
{
    Q_OBJECT
public:
    explicit TscoreWidget(unsigned char _notesCount =1, QWidget* parent = 0);

//    QAction *scoreSettAct, *keySigNameAct;
//    QMenu *contextMenu;
    void setEnableEnharmNotes(bool isEnabled);

signals:
        /** This signal is emited to inform main window and his @param useDblAccidAct
        * to change its isChecked() state */
//    void enableDblAccWasChanged(bool isEnabled);

public slots:
//    void enableDblAccidsSlot(bool isEnabled);
//    void enableKeySigNameSlot(bool isEnabled);
    void whenNoteWasChanged(int index, Tnote note);

protected:
//    void contextMenuEvent(QContextMenuEvent *);

//    void createActions();




};

#endif // TSCOREWIDGET_H
