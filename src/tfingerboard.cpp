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
    matrix.reset();
    if (!gl->GisRightHanded) {
        matrix.translate(width(),0);
        matrix.scale(-1,1);
        painter.setMatrix(matrix);
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
  // FRETS
   // zero fret (upper bridge or HUESO)
    painter.setPen(QPen(QColor("#FFFBF0"),4,Qt::SolidLine)); //#FFFBF0 cream color for hueso
    painter.setBrush(QBrush(QColor("#FFFBF0"),Qt::SolidPattern));
    painter.drawRect(fbRect.x()-6,fbRect.y()+6,5,fbRect.height());
    painter.setPen(QPen(QColor("#FFFBF0"),1,Qt::SolidLine));
    a.setPoints(4, fbRect.x()-8, fbRect.y()+2, fbRect.x()-1, fbRect.y()+2,
               fbRect.x()+strGap/3-1, fbRect.y()-strGap/3,
               fbRect.x()+strGap/3-8, fbRect.y()-strGap/3);
    painter.drawPolygon(a);
 // others frets
    painter.setPen(QPen(QColor("#C0C0C0"),4,Qt::SolidLine)); //#C0C0C0 gray color of frets
        //white color for circles marking 5,7,9... frets
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    for (int i=0; i<gl->GfretsNumber; i++) {
        painter.drawLine(fretsPos[i], fbRect.y()+2, fretsPos[i], height()-fbRect.y()-1);
        if ( i==4 || i==6 || i==8 || i==11 || i==14 || i==16)	{
            painter.setPen(QPen(Qt::black,0,Qt::NoPen));
            painter.drawEllipse(fretsPos[i]-4-(fretsPos[i]-fretsPos[i-1])/2,
                                fbRect.y()+strGap*3-2,8,8);
            //                            fbPainter.drawEllipse(fretsPos[i]-4-(fretsPos[i]-fretsPos[i-1])/2,Ystart+betweenStrings*3-2,8,8);
            painter.setPen(QPen(QColor("#C0C0C0"),4,Qt::SolidLine)); // restore frets' color
        }
    }







}
