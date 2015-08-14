/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#include <nootkacoreglobal.h>
#include "tscoreitem.h"
#include <music/tclef.h>
#include <QPointer>

#define KEY_HEIGHT (14.0) // Height of key signature item

/** 
* This class represents the field between clef
* and notes with key signature accidentals.
*
* User can change key signature by pressing mouse
* and it emits keySignatureChanged signal.
*/
class NOOTKACORE_EXPORT TscoreKeySignature : public TscoreItem
{
  Q_OBJECT
  
public:
    TscoreKeySignature(TscoreScene *scene, TscoreStaff *staff, char keySign = 0);
    
        /** This methods get and set the key signature, and are called
        * only from their parent @class TscoreWidgetSimple as continuation
        * his public methods */
    void setKeySignature(char keySign);
    char keySignature() { return m_keySignature; }
    void setClef(Tclef clef);
		
        /** Returns y coefficient of given note (0 - 7, 0 is c, 1 is d...).
         * It depends on Tclef value*/
    char getPosOfAccid(int noteNr, bool flatKey = false);
		
				/** Returns position point of accidental text in staff coordinates. @p noteNr is [0-7] range */
		QPointF accidTextPos(int noteNr);
		
		void showKeyName(bool showIt);
		
		void setReadOnly(bool readOnly) { m_readOnly = readOnly; if (m_lowKey) m_lowKey->setReadOnly(readOnly); }
		bool readOnly() { return m_readOnly; }
		
				/** It sets background of the note segment. When sets to -1 means transparent - no background. */
		void setBackgroundColor(QColor bg) { m_bgColor = bg; update(); }
		
				/** Static method that calculates scale factor of key signature name appropriate for available space above clef. */
		static void setKeyNameScale(QGraphicsTextItem *keyNameItem);
		static const qreal relatedLine; /** Y position of upper staff line in item coordinates */
  
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual QRectF boundingRect() const;
    
signals:
    void keySignatureChanged();

protected:
		virtual void untouched(const QPointF& scenePos);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    void increaseKey(int step); /** Adds @param step to key value. Only 1 or -1 values are accepted. */
		void updateKeyName();
		
protected slots:
		void onLowKeyChanged();

private:
        /** Array of text items with # or b signs*/
    QGraphicsSimpleTextItem 				*m_accidentals[7];
		QPointer<QGraphicsTextItem>			 m_keyNameText;
    char 												 		 m_keySignature;
		QPointer<TscoreKeySignature>		 m_lowKey;
    
        /** It keeps array of accidental symbol (# or b) positions
        * (in PosY coordinates from TnoteView)
        * @li [0] is position for f# and fb
        * @li [1] c# and
        * @li etc....    */
    static char 										 m_posOfAccid[7];
    static char 										 m_posOfAccidFlats[7];
    Tclef  													 m_clef;
		bool														 m_readOnly;
		QColor 													 m_bgColor;
		int															 m_clefOffset; /** accidental distance from upper staff line depends on clef */

};

#endif // TSCOREKEYSIGNATURE_H
