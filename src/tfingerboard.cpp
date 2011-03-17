#include "tfingerboard.h"
#include "tglobals.h"
#include <QtGui>
#include <QDebug>


extern Tglobals *gl;
/*static*/
const Ttune TfingerBoard::stdTune = Ttune(tr("Standard E A D G B E "), Tnote(3,1,0), Tnote(7,0,0),
                                          Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(3,-1,0));


TfingerBoard::TfingerBoard(QWidget *parent) :
    QGraphicsView(parent)
{
    if (gl->GfingerColor == -1) {
        gl->GfingerColor = palette().highlight().color();
        gl->GfingerColor.setRgb(qRgb(255-gl->GfingerColor.red(),255-gl->GfingerColor.green(),
                                     255-gl->GfingerColor.blue()));
        gl->GfingerColor.setAlpha(200);
    }
    if (gl->Gtune.name == "")
        gl->Gtune = stdTune;

    m_scene = new QGraphicsScene();

    setRenderHint(QPainter::Antialiasing, true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(("background: transparent"));
    setScene(m_scene);
    setMouseTracking(true);

    for (int i=0; i<6; i++) {
        m_strings[i] = new QGraphicsLineItem();
        m_strings[i]->hide();
        m_scene->addItem(m_strings[i]);

        m_fingers[i] = new QGraphicsEllipseItem();
        m_fingers[i]->hide();
        m_fingers[i]->setPen(QPen(palette().highlight().color()));
        m_fingers[i]->setBrush(QBrush(palette().highlight().color(),Qt::SolidPattern));
        m_scene->addItem(m_fingers[i]);

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

void TfingerBoard::resizeEvent(QResizeEvent *){
    m_scene->setSceneRect(0,0,width(),height());
    paint();

}

void TfingerBoard::paint() {
    fbRect = QRect(10, height()/8, (6*width())/7, height()-height()/4);
    fretWidth = ((fbRect.width() + ((gl->GfretsNumber / 2)*(gl->GfretsNumber / 2 +1))
                  + gl->GfretsNumber / 4) / (gl->GfretsNumber+1)) +1;
    strGap = (height()-2*fbRect.y()) / 6;
    fretsPos[0] = fbRect.x() + fretWidth;
    for (int i=2; i<gl->GfretsNumber+1; i++)
        fretsPos[i-1] = fretsPos[i-2]+(fretWidth-(i/2));
    lastFret = fretsPos[gl->GfretsNumber-1];
  // Let's paint
    QPixmap pixmap(size());
    pixmap.fill(QColor(palette().background().color()));

    QPainter painter(&pixmap);
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
    strFont.setPixelSize((int)qRound(0.75*strGap));//setting font for digits
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
        m_workStrings[i]->setPen(QPen(gl->GfingerColor,strWidth+2,Qt::SolidLine));
        m_workStrings[i]->setLine(matrix.map(QLineF(1, fbRect.y()+strGap/2+i*strGap,
                                  width()-1-strGap, fbRect.y()+strGap/2+i*strGap)));
        m_strings[i]->setPen(QPen(palette().highlight().color(),strWidth,Qt::SolidLine));
        m_strings[i]->setLine(matrix.map(QLineF(1, fbRect.y()+strGap/2+i*strGap,
                                  width()-1-strGap, fbRect.y()+strGap/2+i*strGap)));
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
    m_workFinger->setRect(0,0, fretWidth/1.6, qRound(0.6*strGap));
    for (int i=0; i<6; i++)
        m_fingers[i]->setRect(0,0, fretWidth/1.6, qRound(0.7*strGap));
    m_scene->setBackgroundBrush(QBrush(pixmap));

}

void TfingerBoard::mouseMoveEvent(QMouseEvent *event) {
    int strNr = 7, fretNr = 99;
    if ( (event->y() >= fbRect.y()) && (event->y() <= (height()-fbRect.y()-4)) ) {
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
    if (m_curStr != strNr || m_curFret != fretNr) {
        if ( fretNr > 0 && fretNr < 99) { // show finger
            int off = qRound(fretWidth/1.5);
            if (matrix.dx()) off = 4;
            m_workFinger->setPos(matrix.map(QPoint(fretsPos[fretNr-1]-off,
                                 fbRect.y()+strGap*strNr+strGap/5)));
            if (!m_workFinger->isVisible())
                m_workFinger->show();
            if (m_curStr != 7) m_workStrings[m_curStr]->hide();
        } else { // show string line
            m_workFinger->hide();
            if (m_curStr != 7) m_workStrings[m_curStr]->hide();
            if (strNr != 7) m_workStrings[strNr]->show();
        }
        m_curStr = strNr;
        m_curFret = fretNr;
    }
}

void TfingerBoard::mousePressEvent(QMouseEvent *) {
//    qDebug() << QString::fromStdString(posToNote(m_curStr,m_curFret).getName(gl->nameStyleInKeySign));
    Tnote aNote = posToNote(m_curStr,m_curFret);
    setFinger(aNote);
    emit guitarClicked(aNote);
}

Tnote TfingerBoard::posToNote(int str, int fret) {
    return Tnote(gl->Gtune[str+1].getChromaticNrOfNote()+fret);
}

bool TfingerBoard::setFinger(Tnote note, int realStr) {
        bool doShow = true;
        for(int i=0; i<6; i++) { // looking for pos to show
            int diff = note.getChromaticNrOfNote() - gl->Gtune[i+1].getChromaticNrOfNote();
            if ( doShow && diff >= 0 && diff <=gl->GfretsNumber) { // found
                if (diff == 0) { // open string
                    m_fingers[i]->hide();
                    m_strings[i]->show();
                } else { // some fret
                    m_strings[i]->hide();
                    int off = qRound(fretWidth/1.5);
                    if (matrix.dx()) off = 4;
                    m_fingers[i]->setPos(matrix.map(QPoint(fretsPos[diff-1]-off,
                                         fbRect.y()+strGap*i+strGap/5)));
                    m_fingers[i]->show();
                }
                if (!gl->GshowOtherPos || i==realStr-1) {
                    doShow = false;
                }
            } else { // not found on this string or no need to show
                m_fingers[i]->hide();
                m_strings[i]->hide();
            }
        }
}
