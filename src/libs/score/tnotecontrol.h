/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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


#ifndef TNOTECONTROL_H
#define TNOTECONTROL_H

#include "tscoreitem.h"
#include <QPointer>
#include <QLinearGradient>
#include "nootkacoreglobal.h"

class QTimer;
class TscoreNote;
class TscoreStaff;


#if defined (Q_OS_ANDROID)
  #define WIDTH (4.0)
#else
  #define WIDTH (2.5)
#endif



class NOOTKACORE_EXPORT TnoteControl : public TscoreItem
{

	Q_OBJECT
	
public:
	TnoteControl(TscoreStaff* staff, TscoreScene* scene);
	virtual ~TnoteControl();

	TscoreNote* scoreNote() { return m_scoreNote; }
	void setScoreNote(TscoreNote* sn);
	
	void adjustSize(); /** Grabs height from staff and adjust items to it. */
	
      /** Hides controller with given delay time [ms] or with default delay (-1)
       * which is 1000 when touch is enabled and 300 if not.
       * 0 - hides it immediately.  */
  void hideWithDelay(int delay = -1);
	
			/** Adds accidentals symbols to the controller. Detects are double accidentals enabled.
			 * It can be also used to refresh double accidentals state - add/remove them */
	void addAccidentals();

	void setAccidental(int acc); /** Sets accidental on the controller */
	void setEnabled(bool ctrlEnabled) { m_isEnabled = ctrlEnabled; } /** When enabled - it is displayed under cursor (finger) */
	bool isEnabled() { return m_isEnabled; }
	
	
			/** Enables or disables 'plus' and 'minus' buttons.
			 * Notice, when enabled, 'remove' (minus) is displayed only when staff has more notes than one. */
	void enableToAddNotes(bool addEnabled);
	bool notesAddingEnabled() { return m_notesAdding; }
	
			/** Enables or disables 'note name' button. */
	void enableNoteName(bool enableName) { enableName ? m_name->show() : m_name->hide(); }
	bool noteNameEnabled() { return m_name->isVisible(); }
	
	bool isLeftPane() { return m_sharp != 0; } /** True when it has accidentals - as such as left controller has */
	
	void hide();
	
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	virtual QRectF boundingRect() const;
	
signals:
	void nameMenu(TscoreNote* scoreNote);
		
protected:	
	void itemSelected(const QPointF& cPos);
	
	virtual void touched(const QPointF& scenePos);
	virtual void untouched(const QPointF& scenePos);
  
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

protected slots:
	void hideDelayed();
	void showDelayed();

private:
		TscoreNote																*m_scoreNote;
		qreal						 								 					 m_height;
		bool						 								 					 m_isEnabled, m_entered;
		QGraphicsSimpleTextItem										*m_name, *m_cross;
		QGraphicsSimpleTextItem									  *m_dblSharp, *m_sharp, *m_flat, *m_dblFlat;
		QGraphicsRectItem													*m_accidLight, *m_accidGap;
		QLinearGradient														 *m_gradient; // pointer to background gradient
		static QLinearGradient										 m_leftGrad, m_rightGrad;
//Android
		QGraphicsItem															*m_underItem; // Item under mouse
		bool														 					 m_moveNote; // True when note cursor is moved with finger
		int																				 m_currAccid;
		QGraphicsSimpleTextItem 									*m_prevAccidIt;
		bool																			 m_notesAdding, m_nameEnabled;
		bool																			 m_adding; // TRUE when click adds a note
		QTimer																		*m_delayTimer;
		
private:
		QGraphicsSimpleTextItem* createNootkaTextItem(const QString& aText);
		inline qreal centeredX(QGraphicsItem* it); /** X coordinate of centered item position */
		void accidChanged(QGraphicsItem* accidIt);
		void markItemText(QGraphicsSimpleTextItem* textItem);
		
};

#endif // TNOTECONTROL_H
