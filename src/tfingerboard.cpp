#include "tfingerboard.h"
#include "tglobals.h"
#include <QtGui>


extern Tglobals *gl;

TfingerBoard::TfingerBoard(QWidget *parent) :
    QWidget(parent)
{
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setRenderHint(QPainter::Antialiasing, true);
    setMouseTracking(true);
//    setFrameShape(QFrame::NoFrame);
//    setStyleSheet(("background: transparent"));

    if (gl->GfingerColor == -1) {
        gl->GfingerColor = palette().highlight().color();
        gl->GfingerColor.setAlpha(175);
    }




}

void TfingerBoard::paintEvent(QPaintEvent *) {
  // Prepare variables
    fbRect = QRect(10, height()/8, (6*width())/7, height()-height()/4);
    fretWidth = ((fbRect.width() + ((gl->GfretsNumber / 2)*(gl->GfretsNumber / 2 +1))
                  + gl->GfretsNumber / 4) / (gl->GfretsNumber+1)) +1;
    strGap = (height() - 2*fbRect.x()) / 6;
    fretsPos[0] = fbRect.x() + fretWidth;
    for (int i=2; i<gl->GfretsNumber+1; i++)
        fretsPos[i-1] = fretsPos[i-2]+(fretWidth-(i/2));
    lastFret = fretsPos[gl->GfretsNumber-1];
  // Let's paint
    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing , true);
    painter.setWindow(0,0,width(),height());
//    matrix.reset();
//    if (!gl->GisRightHanded) {
//        matrix.translate(width(),0);
//        matrix.scale(-1,1);
//    }

  // Guitar body
    painter.setPen(QPen(QColor("#ECC93E")));
    painter.setBrush(QBrush(QPixmap(":/picts/body.png")));
    painter.drawRect(fbRect.x()+fretWidth*12, 0, width()-fbRect.x()+fretWidth*12-37, height());

}
