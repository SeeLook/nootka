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

#ifndef TSCOREKEYSIGNATURE_H
#define TSCOREKEYSIGNATURE_H

#include "tscoreitem.h"
#include <tclef.h>

    /** This class represents the field between clef
    * and notes with key signature acidentals.
    *
    * User can change key signature by mouse wheel moving,
    * and it emits keySignatureChanged signal.
    */
class TscoreKeySignature : public TscoreItem
{
  Q_OBJECT
  
public:
    TscoreKeySignature(TscoreScene *scene, TscoreStaff *staff, char keySign = 0);
    ~TscoreKeySignature();
    
        /** This metods get and set the key signature, and are called
        * only from their parent @class TscoreWidgetSimple as continuation
        * his public methods */
    void setKeySignature(char keySign);
    char keySignature() { return m_keySignature; }
    void setClef(Tclef clef);
        /** Returns y coefficient of given note (0 - 7, 0 is c, 1 is d...).
         * It depends on Tclef value*/
    char getPosOfAccid(int noteNr, bool flatKey = false);
		
		void showKeyName(bool showIt);
		
		void setReadOnly(bool readOnly) { m_readOnly = readOnly; }
		bool readOnly() { return m_readOnly; }
  
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual QRectF boundingRect() const;
    
signals:
    void keySignatureChanged();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
        /** Adds @param step to key value. Only 1 or -1 values are accepted. */
    void increaseKey(int step);
		void updateKeyName();

private:
        /** Array of text items with # or b signs*/
    QGraphicsSimpleTextItem *m_accidentals[7];
		QGraphicsTextItem *m_keyNameText;
    char m_keySignature;
    qreal m_height;
    
        /** It keeps array of accidental symbol (# or b) positions
        * (in PosY coordinates from TnoteView)
        * @li [0] is position for f# and fb
        * @li [1] c# and
        * @li etc....    */
    static char m_posOfAccid[7];
    static char m_posOfAccidFlats[7];
    Tclef  m_clef;
		bool	m_readOnly;

};

#endif // TSCOREKEYSIGNATURE_H
