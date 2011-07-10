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


#ifndef TFIRSTRUNWIZZARD_H
#define TFIRSTRUNWIZZARD_H

#include <QDialog>


class QStackedLayout;
class QRadioButton;
class QCheckBox;

class TfirstRunWizzard : public QDialog
{
    Q_OBJECT
public:
    explicit TfirstRunWizzard(QWidget *parent = 0);
    static QString nextText;

signals:

public slots:

private:
    QPushButton *skipButt, *nextButt, *prevButt;
    QStackedLayout *pagesLay;

private slots:
    void nextSlot();
    void prevSlot();


};

// page 1 is about dialog
// page 2 is QLabel

class Tpage_3 : public QWidget
{
    Q_OBJECT
public:
    explicit Tpage_3(QWidget *parent = 0);

private:
    QRadioButton *isBradio, *isHradio;
    QCheckBox *dblAccChB, *enharmChB;


};

#endif // TFIRSTRUNWIZZARD_H
