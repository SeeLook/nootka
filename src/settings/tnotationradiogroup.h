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


#ifndef TNOTATIONRADIOGROUP_H
#define TNOTATIONRADIOGROUP_H

#include "tnote.h"
#include <QGroupBox>

/**
@author Tomasz Bojczuk
*/

class QRadioButton;
class QButtonGroup;

class TnotationRadioGroup: public QGroupBox
{
        Q_OBJECT
public:
        explicit TnotationRadioGroup(Tnote::EnameStyle _notation, QWidget *parent = 0);


        Tnote::EnameStyle notation;

        static const QString strNorsk;
        static const QString strItal;
        static const QString strDeutsch;
        static const QString strEnglish;
        static const QString strNeder;
        static const QString strNorskExampl;
        static const QString strItalExampl;
        static const QString strDeutschExampl;
        static const QString strEnglishExampl;
        static const QString strNederExampl;

        Tnote::EnameStyle getNameStyle();

signals:
        void noteNameStyleWasChanged (Tnote::EnameStyle);

public slots:
        void noteNameStyleWasClicked();
        void seventhNoteWasChanged(bool isB);

private:
        QRadioButton *norskButt;
        QRadioButton *deutschButt;
        QRadioButton *italianoButt;
        QRadioButton *englishButt;
        QRadioButton *nederlButt;
        QButtonGroup *buttonGroup;

};

#endif // TNOTATIONRADIOGROUP_H
