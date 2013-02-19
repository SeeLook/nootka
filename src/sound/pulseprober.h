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


#ifndef PULSEPROBER_H
#define PULSEPROBER_H

#include <QLabel>

    /** Search dir @param dirName with @param keyWord
     * and returns whole path to found dir or empty string. */
QString searchDir(QString dirName, QString keyWord);
    /** Checks for pulseaudio plugin library in Qt plugins dirs. */
bool hasPulsePlugin();
    /** Checks Linux system for necessarity of runnung PulseAudio.
     * Returns true when pulse works or is not need 
     * or false when audio won't work without it. */
bool checkForPulse();

//##########################################################################################
//#######################     TpulseWarring ################################################
//##########################################################################################

/** TpulseWarring is a widget with information about broken pulseaudio. */
class TpulseWarring : public QLabel
{
  
public:
    explicit TpulseWarring(QWidget* parent = 0);
    virtual ~TpulseWarring() {}
  
};


#endif // PULSEPROBER_H
