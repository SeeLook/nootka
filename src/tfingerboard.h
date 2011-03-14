#ifndef TFINGERBOARD_H
#define TFINGERBOARD_H

//#include <QWidget>
#include <QGraphicsView>

class TfingerBoard : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TfingerBoard(QWidget *parent = 0);

    void settingsWasChanged() { repaint(); }

signals:

public slots:

protected:
    QMatrix matrix;
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);

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
    short prevStr[6], prevFret[6];
        /** @param fretsPos  stores X positions of frets in global widget coordinates */
    short fretsPos[24];

    QGraphicsScene *m_scene;
    QGraphicsEllipseItem *m_workFinger;
    int m_strNr, m_fretNr, m_curStr, m_curFret;
    QGraphicsLineItem *m_workStrings[6];

    /** */
    /** */


};

#endif // TFINGERBOARD_H
