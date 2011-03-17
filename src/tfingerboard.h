#ifndef TFINGERBOARD_H
#define TFINGERBOARD_H

#include <QWidget>
#include <QGraphicsView>
#include "tnote.h"
#include "ttune.h"


class TfingerBoard : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TfingerBoard(QWidget *parent = 0);

    void settingsWasChanged() { paint(); }

    static const Ttune stdTune;
        /** It paints fingerprint on the fretboard in place where note is.
        * If globals GshowOtherPos is true all possibilities are shown.
        * If @param realStr is set is shown only on pointed string (of any).
        * It returns true if something is shown.*/
    bool setFinger(Tnote note, int realStr = 7);
//    void resize();

signals:
    void guitarClicked(Tnote note);

public slots:

protected:

    void paint();
    Tnote posToNote(int str, int fret);

    QMatrix matrix;
    void resizeEvent(QResizeEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
protected slots:

private:
    // I HAVE TOO SHORT HANDS TO PLAY WITH 'm_' BEFORE EACH PRIVATE VARIABLE :((((

        /** Represents top left positions and size of fingerboard */
    QRect fbRect;
        /** Distance between strings */
    short strGap;
        /** Average width of fret */
    short fretWidth;
        /** Position of the last fret (in whole widget coordinates) */
    short lastFret;
        /** Actual position of cursor over the guitar in strings/frets coordinates */
    short curStr, curFret;
        /** In those variables are strored previous nubers of strings and frets.
          *This values are needed to hide (or remove) provious preview on the guitar */
//    short prevStr[6], prevFret[6];
        /** It keeps position of selected fingerprint. x() is string and y() is fret.*/
    QPoint m_fingerPos;
        /** @param fretsPos  stores X positions of frets in global widget coordinates */
    short fretsPos[24];

//    QGraphicsView *m_view;
//    TkindView *m_view;
    QGraphicsScene *m_scene;
    QGraphicsEllipseItem *m_workFinger, *m_fingers[6];
    int m_strNr, m_fretNr, m_curStr, m_curFret;
    QGraphicsLineItem *m_workStrings[6], *m_strings[6];

    /** */
    /** */


};

#endif // TFINGERBOARD_H
