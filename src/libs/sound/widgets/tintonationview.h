/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#ifndef TINTONATIONVIEW_H
#define TINTONATIONVIEW_H

#include "tabstractsoundview.h"
#include "nootkacoreglobal.h"


#define INT_FACTOR (1.2)

class QTimer;
class QComboBox;

class NOOTKASOUND_EXPORT TintonationView : public TabstractSoundView
{
  
  Q_OBJECT
  
public:
			/** Eaccuracy describes difference between a perfect pitch and detected one.
				* As long as well tuned guitar keeps @p e_perfect ar even @ e_paranoid
				* For human voice @p e_normal or @p e_sufficient is acceptable. */
	enum Eaccuracy {
			e_noCheck = 0, // ± 0.41 - 0.49
			e_dogHowl = 1, // ± 0.31 - 0.40
			e_sufficient = 2, // ± 0.21 - 0.3
			e_normal = 3, // ± 0.11 - 0.2
			e_perfect = 4, // ± 0.06 - 0.1
			e_paranoid = 5 // ± 0.05
	};

	explicit TintonationView(int accuracy, QWidget *parent = 0);
	virtual ~TintonationView();

			/** Determines how wide will be range of green color (in tune) on the view.
				* @p e_noCheck - widget disables
				* @p e_dogHowl  ± 0.31 - 0.40
				* @p e_sufficient  ± 0.21 - 0.3
				* @p e_normal ± 0.11 - 0.2
				* @p e_perfect ± 0.06 - 0.1
				* @p e_paranoid ± 0.05  */
	void setAccuracy(int accuracy);
	Eaccuracy accuracy() { return m_accuracy; }

      /** Decides whether intonation is active for changes or not. */
  void setAccuracyChangeEnabled(bool accEnabled);
  bool accuracyChangeEnabled() { return m_enableAccurChange; }

			/** Starts animation displaying correction of unclear sound.  */
	void outOfTuneAnim(float outTune, int duration);

			/** Returns a threshold for given accuracy. Above that value notes are out-of-tune. */
	static float getThreshold(Eaccuracy acc);
	static float getThreshold(int accInteger);

			/** Returns difference of unclear pitch (float point part)
				* multiplied by some size factor. */
	static float getPitchDiff(float pitch) { return qBound(-0.49, (pitch - (float)qRound(pitch)) * INT_FACTOR, 0.49); }

signals:
	void animationFinished();
	void accuracyChanged();

public slots:
	void pitchSlot(float pitch);

protected:
	virtual void paintEvent(QPaintEvent* );
	virtual void resizeEvent(QResizeEvent*);
	virtual void enterEvent(QEvent*);
	virtual void leaveEvent(QEvent*);
	virtual void mouseMoveEvent(QMouseEvent* e);
	virtual void mousePressEvent(QMouseEvent* e);

protected slots:
	void animationSlot();

private:
	QList<QColor>   m_tickColors;
	int             m_ticksCount;
	float           m_pitchDiff;
	int             m_noteX;
	float           m_hiTickStep;

	Eaccuracy       m_accuracy;
	float           m_accurValue, m_currAccurInAnim, m_animStep, m_outOfTune;
	QTimer				 *m_timer;
  bool            m_enableAccurChange;
	bool						m_entered, m_overNote;
};

/**
 * This is QLabel with combo box in single line.
 * ComboBox contains accuracy levels for intonation check.
*/
class NOOTKASOUND_EXPORT TintonationCombo : public QWidget
{
	Q_OBJECT
	
public:
	explicit TintonationCombo(QWidget* parent);
	
			/** Returns translations of given accuracy level.  */
	static QString intonationAccuracyTr(TintonationView::Eaccuracy accur);
	static QString centsText() { return tr("cents", 
		"unit of measure used for musical intervals. This text always is used in context: ' ±	[5, 10, 20, 30] cents ' "); }
	
	QComboBox *accuracyCombo;
	
	
};

#endif // TINTONATIONVIEW_H
