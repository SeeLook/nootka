#ifndef TNOTEVIEW_H
#define TNOTEVIEW_H

#include <QGraphicsView>
//#include <tscorewidgetsimple.h>




class QGraphicsEllipseItem;
class QGraphicsScene;
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;
class TscoreWidgetSimple;

/** @short This class is based on QGraphicsView widget and provides to show single note on the score.
* It supports mouseMoveEvent, which moves the note and wheelEvent which adds accidentals.
* @param left is position of QGraphicsView on TscoreWidgetSimple multiple by scoreCanvasWidth
* When note is changed, it emits @p noteIsClicked signal.
@author Tomasz Bojczuk
*/
class TnoteView : public QGraphicsView
{
    friend class TscoreWidgetSimple;

    Q_OBJECT
public:
    explicit TnoteView(int _index, TscoreWidgetSimple * parent = 0);
    void moveNote(int pos);
    int index() { return m_index; }
    void setAmbitus(int min, int max);
    void setNote(int notePos, char accNr);
    void hideNote();
    void setColor(QColor color);


        /** This return value of @li -2 is bb @li 1 is #
         * @li etc... */
    int accidental() {return m_accidental;}
    int notePos() { return m_mainPosY; }

signals:
        /** This sigmal is emited when user moves the mouse wheel.
         * Acidental is changed, so adequate bitton has to change its status
         * @param accNr   */
   void accidWasChanged (int);

   void noteWasClicked (int);

public slots:

protected:
//    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
//    bool event(QEvent *event);

    void resize(int co);
        /** This method is used by TscoreWidgetSimple when buttons with accidetals
        * are clicked to set apropirate accidental with m_workNote */
    void setAccidText(int accNr);
//    void setDblAccFuse(char fuse) { m_dblAccFuse = fuse; } //default 1 - no dblaccids

private:
    QGraphicsScene *m_scene;
    QGraphicsEllipseItem *m_workNote, *m_mainNote;
    QGraphicsLineItem *m_upLines[7], *m_mainUpLines[7], *m_mainDownLines[5], *m_downLines[5];
    QGraphicsSimpleTextItem *m_workAccid, *m_mainAccid;
    QColor m_workColor, m_mainColor;
    int m_workPosY, m_mainPosY;
    int m_coeff;
    int m_curentAccid, m_accidental;
    int m_index;
        /** Represents range (ambitus) of notes on score */
    int m_ambitMin, m_ambitMax;
        /** This points at array @param TscoreWidget::accInKeyArr[7],
        * which keeps information
        * about adequate accidental in key signature.
        * F.e. @p keyAcidsArr[0] is -1 for C# in D-maj key*/
    char *m_accInKeyPtr;
        /** To calculate correct positnion of accidental symbol on the staff */
    int m_accTextOffset;
        /** It is @p 2 if double accidentals are enabled and @p 1 if not*/
    char *m_dblAccFusePtr;
        /** Returns QString with accidental symbol*/
    QString getAccid(int accNr);
    void hideWorkNote();

};

#endif // TNOTEVIEW_H
