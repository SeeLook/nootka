/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
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

#ifndef TMELODYVIEW_H
#define TMELODYVIEW_H

#include <QGraphicsView>

class TscoreStaff;
class Tmelody;



/** 
 * @class TmelodyView is QGraphicsView with multiple staves of given melody.
 * It is intended to representing melodies in Nootka charts.
 */
class TmelodyView : public QGraphicsView
{
  
  Q_OBJECT
  
public:
  TmelodyView(Tmelody* melody, QWidget* parent = 0);
  
  void markMistakes(QList<quint32> mistakes); /** Marks notes in the score with colors corresponding to mistake. */
  void clearMistakes(); /** Clears marked note heads. */
  void showStringNumbers(bool strNrOn); /** Adds sting numbers to every note (except (1) - first string) */
  
protected:
  virtual void resizeEvent(QResizeEvent* event);
  
private:
  Tmelody                         *m_melody;
  QList<TscoreStaff*>              m_staves;
  const int                        m_maxNotes; /** max notes number in single staff. */
};

#endif // TMELODYVIEW_H
