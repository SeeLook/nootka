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


#ifndef TSCALEFILE_H
#define TSCALEFILE_H


#include <QString>


/** Takes care about file with musical scale.
 * Constructor does nothing. File is loaded by invokeing loadAudioData() 
 */
class TscaleFile
{

public:
    TscaleFile(QString &path);
    virtual ~TscaleFile();
    
        /** Loads wav file with scale to m_audioArr. If everything is ok returns true */
    bool loadAudioData();
        /** Unloads audio data from an array m_audioArr */
    void deleteData();
    qint16 *audioArr() { return m_audioArr; }
    
        /** Returns single wav sample (16 bit) from array.
         * !!! It doesn't check does element exist in array !!!*/
    qint16 getSample(int offset) { return m_audioArr[offset]; }
    
private:
    qint16 *m_audioArr;
        /** Path to wav file with sounds */
    QString m_wavFile;
};

#endif // TSCALEFILE_H
