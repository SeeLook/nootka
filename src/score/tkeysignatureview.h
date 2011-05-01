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

#ifndef TKEYSIGNATUREVIEW_H
#define TKEYSIGNATUREVIEW_H

#include <QGraphicsView>
#include "tscorewidgetsimple.h"


class TscoreWidgetSimple;
class QGraphicsScene;
class QGraphicsSimpleTextItem;

    /** @short This class represents the field between clef
    * and notes with key signature acidentals.
    *
    * User can change key signature by mouse wheel moving,
    * and @class TKeySignView generated keySignWasChanged
    * signal with @p char keySign information.
    @author Tomasz Bojczuk  <tomaszbojczuk@gmail.com>
    *
    */
class TkeySignatureView : public QGraphicsView
{
    friend class TscoreWidgetSimple;

    Q_OBJECT
public:

          /** Constructs TKeySignView class instance
          * @param _keySign The key signature (default is 0 - C-maj)
          * @param parent its parent (have to be TscoreWidget class)
          *        */
    explicit TkeySignatureView(TscoreWidgetSimple *parent = 0, char _keySign = 0);

    char keySignature() { return m_keySignature; }
            /** It sets fuses to block mouse wheel event and stop adding more flats or sharps*/
    void setKeysFuse(char flatFuse, char sharpFuse);



signals:
        /** Is called to inform @class TscoreWidgetSimple */
    void keySignWasChanged();

public slots:

protected:
    void wheelEvent(QWheelEvent *event);
            /** This metods get and set the key signature, and are called
            * only from their parent @class TscoreWidgetSimple as continuation
            * his public methods */
    void setKeySignature(char keySign);
             /** @class TscoreWidgetSimple as a parent has to take care about resizing process
             * so it calls this function in his resizeEvent and put @param coeff parameter */
    void resize(int co);
        /** It shows or hides key signature name depend on global settings.*/
    void showKeyName();



private:
    QGraphicsScene *m_scene;
        /** Array of text items with # or b signs*/
    QGraphicsSimpleTextItem *m_accidentals[7];
    QGraphicsSimpleTextItem *m_keyNameText;

    int m_coeff;
    char m_keySignature;
        /** It keeps array of accidental symbol (# or b) positions
        * (in PosY coordinates from TnoteView)
        * @li [0] is position for f# and fb
        * @li [1] c# and
        * @li etc....    */
    char m_posOfAccid[7];
                /** This points at array @param TscoreWidget::accInKeyArr[7],
                    * which keeps information
                    * about adequate accidental in key signature.
                    * F.e. @p keyAcidsArr[0] is -1 for C# in D-maj key*/
    char *m_accInKeyPtr;
            /** To calculate correct positnion of accidental symbol on the staff */
    int m_accTextOffset;
    char m_flatKeyFuse, m_sharpKeyFuse;

};

#endif // TKEYSIGNATUREVIEW_H
