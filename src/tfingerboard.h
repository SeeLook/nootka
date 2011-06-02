#ifndef TFINGERBOARD_H
#define TFINGERBOARD_H

#include <QWidget>
#include <QGraphicsView>
#include "tnote.h"
#include "ttune.h"
#include "tfingerpos.h"


class TfingerBoard : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TfingerBoard(QWidget *parent = 0);

    void acceptSettings();

        /** It paints fingerprint on the fretboard in place where note is.
        * If globals GshowOtherPos is true all possibilities are shown.
        * If @param realStr is set is shown only on pointed string (if any).
        * It returns true if something is shown.*/
    void setFinger(Tnote note);
    void setFinger(TfingerPos pos);
        /** Returns selected finger position */
    TfingerPos getfingerPos() { return m_fingerPos; }
    void askQuestion(TfingerPos pos);
    void clearFingerBoard();

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
    bool event(QEvent *event);
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
        /** It keeps position of selected fingerprint.*/
    TfingerPos m_fingerPos;
        /** @param fretsPos  stores X positions of frets in global widget coordinates */
    short fretsPos[24];

    QGraphicsScene *m_scene;
    QGraphicsEllipseItem *m_workFinger, *m_fingers[6], *m_questFinger;
    int m_strNr, m_fretNr, m_curStr, m_curFret;
    QGraphicsLineItem *m_workStrings[6], *m_strings[6];
        /** Keeps selected note*/
    Tnote m_selNote;

    void paintFinger(QGraphicsEllipseItem *f, char strNr, char fretNr);

    /** */
    /** */


};

#endif // TFINGERBOARD_H
