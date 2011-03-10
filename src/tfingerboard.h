#ifndef TFINGERBOARD_H
#define TFINGERBOARD_H

#include <QWidget>

class TfingerBoard : public QWidget
{
    Q_OBJECT
public:
    explicit TfingerBoard(QWidget *parent = 0);



signals:

public slots:

protected:
//    QMatrix matrix;
    void paintEvent(QPaintEvent *);

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

    /** */
    /** */


};

#endif // TFINGERBOARD_H
