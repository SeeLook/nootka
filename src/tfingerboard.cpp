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
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setWindow(0,0,width(),height());
//    matrix.reset();
    if (!gl->GisRightHanded) {
        painter.translate(width(),0);
        painter.scale(-1,1);
    }

  // Guitar body
    painter.setPen(QPen(QColor("#ECC93E")));
    painter.setBrush(QBrush(QPixmap(":/picts/body.png")));
    painter.drawRect(fbRect.x()+fretWidth*12, 0, width()-fbRect.x()+fretWidth*12-37, height());
    painter.setPen(QPen(Qt::red,10,Qt::SolidLine));
    painter.setBrush(QBrush(QColor("#404040"),Qt::SolidPattern));
    painter.drawEllipse(lastFret,0-fbRect.y(),height()+2*fbRect.y(),height()+2*fbRect.y());
  // FINGERBOARD
    painter.setPen(QPen(Qt::black,0,Qt::NoPen));
    painter.setBrush(QBrush(Qt::black,Qt::SolidPattern));
    QPolygon a;
    a.setPoints(5, fbRect.x()+2, fbRect.y()-strGap/3,
                fbRect.x()+fbRect.width()+strGap/3, fbRect.y()-strGap/3,
                fbRect.x()+fbRect.width()+strGap/3, height()-fbRect.y()-strGap/3,
                fbRect.x()+fbRect.width(), height()-fbRect.y(),
                fbRect.x(), height()-fbRect.y());
    painter.drawPolygon(a);
    painter.setBrush(QBrush(QPixmap(":/picts/fingbg.png")));
    painter.drawRect(fbRect);
//               5,Xstart+2,Ystart-betweenStrings/3,
//    Xstart+fbLehgth+betweenStrings/3,Ystart-betweenStrings/3,
//    Xstart+fbLehgth+betweenStrings/3,height()-Ystart-betweenStrings/3,
//    Xstart+fbLehgth,height()-Ystart,
//    Xstart,height()-Ystart); fbPainter.setBrush(QBrush(QColor(0,0,0),QPixmap("/home/tomek/DEVELOP/Kdevelop/nutouk/pixmaps/fingbg.png")));
////                fbPainter.drawRect(Xstart,Ystart,fbLehgth,height()-2*Ystart);//AND ORDINARY FINGERBOARD





}
