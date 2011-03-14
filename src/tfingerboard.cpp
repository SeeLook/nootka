#include "tfingerboard.h"
#include "tglobals.h"
#include <QtGui>
#include <QDebug>


extern Tglobals *gl;

TfingerBoard::TfingerBoard(QWidget *parent) :
    QWidget(parent)
{
    if (gl->GfingerColor == -1) {
        gl->GfingerColor = palette().highlight().color();
        gl->GfingerColor.setRgb(qRgb(255-gl->GfingerColor.red(),255-gl->GfingerColor.green(),
                                     255-gl->GfingerColor.blue()));
        gl->GfingerColor.setAlpha(175);
    }

    m_scene = new QGraphicsScene();
    m_view = new QGraphicsView(m_scene,this);
    m_view->setScene(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing, true);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setStyleSheet(("background: transparent"));
    m_view->setMouseTracking(false);
    m_view->installEventFilter(this);
    m_scene->installEventFilter(this);


    setMouseTracking(true);

    for (int i=0; i<6; i++) {
        m_workStrings[i] = new QGraphicsLineItem();
        m_workStrings[i]->hide();
        m_scene->addItem(m_workStrings[i]);
    }

    m_workFinger = new QGraphicsEllipseItem();
    m_workFinger->hide();
    m_workFinger->setPen(QPen(gl->GfingerColor));
    m_workFinger->setBrush(QBrush(gl->GfingerColor,Qt::SolidPattern));
    m_scene->addItem(m_workFinger);

    m_curStr = 7; // none
    m_curFret = 99; // none


}

void TfingerBoard::paintEvent(QPaintEvent *) {
//    m_view->setGeometry(0,0,width(),height());
    m_scene->setSceneRect(0,0,width(),height());

  // Prepare variables
    fbRect = QRect(10, height()/8, (6*width())/7, height()-height()/4);
    fretWidth = ((fbRect.width() + ((gl->GfretsNumber / 2)*(gl->GfretsNumber / 2 +1))
                  + gl->GfretsNumber / 4) / (gl->GfretsNumber+1)) +1;
    strGap = (height()-2*fbRect.y()) / 6;
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
//    painter.drawRect(fbRect.x()+fretWidth*12, 0, width()-fbRect.x()+fretWidth*12-37, height());
    painter.drawRect(fretsPos[11], 0, width()-fretsPos[11]-37, height());
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
  // STRINGS
    QFont strFont = font();
    strFont.setPixelSize((int)qreal(0.75*strGap));//setting font for digits
    painter.setFont(strFont);
    QColor strColor;
    int strWidth; //width of the string depends on its number
    painter.setBrush(QBrush(Qt::NoBrush));
    for (int i=0; i<6; i++) {
        if (i < 2) {
            strColor = Qt::white;
            strColor.setAlpha(175);
            strWidth = 1;
        } else
            if (i==2) {
                strColor = Qt::white;
                strColor.setAlpha(125);
                strWidth = 2;
            } else
                if ( (i==3) || (i==4) ) {
                    strColor = QColor("#C29432"); //#C29432 gold color for bass strings
                    strColor.setAlpha(255);
                    strWidth = 2;
                } else {
                    strColor = QColor("#C29432");
                    strWidth = 3;
                }
//     drawing main strings
        painter.setPen(QPen(strColor,strWidth,Qt::SolidLine));
        painter.drawLine(1, fbRect.y()+strGap/2+i*strGap,
                         width()-1-strGap, fbRect.y()+strGap/2+i*strGap);
        m_workStrings[i]->setPen(QPen(gl->GfingerColor,strWidth,Qt::SolidLine));
        m_workStrings[i]->setLine(1, fbRect.y()+strGap/2+i*strGap,
                                  width()-1-strGap, fbRect.y()+strGap/2+i*strGap);

    //     markStrings[i]->setPoints(1,Ystart+betweenStrings/2+i*betweenStrings,
    //                                                    width()-1-
    //        betweenStrings,Ystart+betweenStrings/2+i*betweenStrings);
  // drawing digits of strings in circles
        painter.setPen(QPen(strColor,1,Qt::SolidLine));
        int wd40;
        if (!gl->GisRightHanded) {
            painter.scale (-1,1);
            painter.translate(-width(),0);
            wd40 = 1;
        }else
            wd40 = width()-1-strGap;
        painter.drawEllipse(wd40, fbRect.y()+i*strGap, strGap-1, strGap-1);
        painter.setPen(QPen(Qt::green,1,Qt::SolidLine));// in green color
        if (gl->GisRightHanded)
            painter.drawText(width()-((int)qreal(strGap*0.75)),
                             fbRect.y()+(int)qreal(strGap*(0.75+i)), QString::number(i+1));
          else {
              painter.drawText((int)qreal(strGap*0.28),
                               fbRect.y()+(int)qreal(strGap*(0.75+i)), QString::number(i+1));
              painter.translate(width(),0);
              painter.scale (-1,1);
          }
  // shadow of the strings
          painter.setPen(QPen(Qt::black,strWidth,Qt::SolidLine));
          painter.drawLine(fbRect.x()+1, fbRect.y()+strGap/2+i*strGap+3+strWidth,
                           fbRect.x()+fbRect.width()-1, fbRect.y()+strGap/2+i*strGap+3+strWidth);
          painter.setPen(QPen(Qt::black,1,Qt::SolidLine)); //on the fingerboard
          painter.drawLine(fbRect.x()-8, fbRect.y()+strGap/2+i*strGap-2,
                           fbRect.x(), fbRect.y()+strGap/2+i*strGap-2);
          painter.drawLine(fbRect.x()-8, fbRect.y()+strGap/2+i*strGap+strWidth-1,
                           fbRect.x()-1, fbRect.y()+strGap/2+i*strGap+strWidth-1);

    }
    m_workFinger->setRect(0,0, fretWidth/2, (int)qreal(0.66*strGap));
//    fingers[i]->setSize(fretWidth/2,(int)round(0.66*betweenStrings));

}

void TfingerBoard::mouseMoveEvent(QMouseEvent *event) {
    int strNr = 7, fretNr = 99;
    if ( (event->y() >= fbRect.y()) && (event->y() <= (height()-fbRect.y()-4)) ) {
        qDebug()<<"kuku";
        int tx, ty;
        matrix.map(event->x(), event->y(), &tx, &ty);
        strNr = (ty-fbRect.y())/strGap;
        if (tx < fbRect.x() || tx > lastFret /*or some mouse button*/ )
            fretNr = 0;
        else {
            for (int i=0; i<gl->GfretsNumber; i++) {
                if (tx <= fretsPos[i]) {
                    fretNr = i+1;
                    break;
                }
            }
        }
    }

    qDebug() << strNr << " " << fretNr;
    qDebug() << m_curStr << " cur: " << m_curFret;
    if (m_curStr != strNr || m_curFret != fretNr) {

        if ( fretNr > 0 && fretNr < 99) { // show finger
            m_workFinger->setPos(fretsPos[fretNr-1]-fretWidth/4-4,
                                 fbRect.y()+strGap*strNr+strGap/2);
            if (!m_workFinger->isVisible())
                m_workFinger->show();
        } else { // show string line

            m_workFinger->hide();
            if (m_curStr != 7) m_workStrings[m_curStr]->hide();
            if (strNr != 7) m_workStrings[strNr]->show();
        }
        m_curStr = strNr;
        m_curFret = fretNr;
    }
}

bool TfingerBoard::eventFilter(QObject *target, QEvent *event) {
//    if ( event->type() == QEvent::Move)
        qDebug()<< event->type();

    return QWidget::eventFilter(target,event);
}
