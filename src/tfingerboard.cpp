/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tfingerboard.h"
#include "tglobals.h"
#include <QtGui>
//#include <QDebug>

extern Tglobals *gl;


TfingerBoard::TfingerBoard(QWidget *parent) :
    QGraphicsView(parent)
{
    if (gl->GfingerColor == -1) {
        gl->GfingerColor = gl->invertColor(palette().highlight().color());
        gl->GfingerColor.setAlpha(200);
    }
    if (gl->GselectedColor == -1) {
        gl->GselectedColor = palette().highlight().color();
    }

    m_scene = new QGraphicsScene();

    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(("background: transparent"));
    setScene(m_scene);
    setMouseTracking(true);
    setStatusTip(tr("Select string or fret and click to see it in the score."));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (int i=0; i<6; i++) {
        m_strings[i] = new QGraphicsLineItem();
        m_strings[i]->hide();
        m_scene->addItem(m_strings[i]);

        m_fingers[i] = new QGraphicsEllipseItem();
        m_fingers[i]->hide();
        m_fingers[i]->setPen(QPen(gl->GselectedColor));
        m_fingers[i]->setBrush(QBrush(gl->GselectedColor, Qt::SolidPattern));
        m_scene->addItem(m_fingers[i]);

        m_workStrings[i] = new QGraphicsLineItem();
        m_workStrings[i]->hide();
        m_scene->addItem(m_workStrings[i]);
    }

    m_workFinger = new QGraphicsEllipseItem();
    m_workFinger->hide();
//    m_workFinger->setPen(QPen(gl->GfingerColor));
    m_workFinger->setPen(QPen(QBrush(Qt::transparent), 5));
    m_workFinger->setBrush(QBrush(gl->GfingerColor, Qt::SolidPattern));
    m_scene->addItem(m_workFinger);

    m_curStr = 7; // none
    m_curFret = 99; // none
    m_selNote = Tnote(0,0,0);
    m_questFinger = 0;
    m_questString = 0;
    m_questMark = 0;
    m_rangeBox1 = 0;
    m_rangeBox2 = 0;
	m_isDisabled = false;
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
    if (lastFret > (fbRect.width()+10))
        fbRect.setWidth(lastFret - 8);
  // Let's paint
    QPixmap pixmap(size());
    pixmap.fill(QColor(palette().background().color()));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setWindow(0,0,width(),height());
    resetTransform();
    if (!gl->GisRightHanded) {
        translate(width(), 0);
        scale(-1, 1);
    }
  // Guitar body
    painter.setPen(QPen(QColor("#ECC93E")));
    painter.setBrush(QBrush(QPixmap(gl->path+"picts/body.png")));
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
    painter.setBrush(QBrush(QPixmap(gl->path+"picts/fingbg.png")));
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
        m_workStrings[i]->setPen(QPen(gl->GfingerColor, strWidth+2, Qt::SolidLine));
        m_workStrings[i]->setLine(1, fbRect.y()+strGap/2+i*strGap, width()-1-strGap,
                                  fbRect.y()+strGap/2+i*strGap);
        m_strings[i]->setPen(QPen(gl->GselectedColor, strWidth, Qt::SolidLine));
        m_strings[i]->setLine(m_workStrings[i]->line());
  // drawing digits of strings in circles
        painter.setPen(QPen(strColor, 1, Qt::SolidLine));
        int wd40;
        if (!gl->GisRightHanded) {
            painter.scale (-1, 1);
            painter.translate(-width(), 0);
            wd40 = 1;
        } else
            wd40 = width()-1-strGap;
        painter.drawEllipse(wd40, fbRect.y()+i*strGap, strGap-1, strGap-1);
        painter.setPen(QPen(Qt::green,1,Qt::SolidLine));// in green color
        if (gl->GisRightHanded)
            painter.drawText(width()-((int)qreal(strGap*0.75)),
                             fbRect.y()+(int)qreal(strGap*(0.75+i)), QString::number(i+1));
          else {
              painter.drawText((int)qreal(strGap*0.28),
                               fbRect.y()+(int)qreal(strGap*(0.75+i)), QString::number(i+1));
              painter.translate(width(), 0);
              painter.scale (-1, 1);
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
//    m_workFinger->setRect(0,0, fretWidth/1.7, qRound(0.6*strGap));
    m_workFinger->setRect(0,0, fretWidth/1.6, qRound(0.7*strGap));
    for (int i=0; i<6; i++)
        m_fingers[i]->setRect(m_workFinger->rect());

    if (m_questFinger) {
        m_questFinger->setRect(m_workFinger->rect());
        paintFinger(m_questFinger, m_questPos.str()-1, m_questPos.fret());
    }
    if (m_questString)
        m_questString->setLine(m_strings[m_questPos.str()-1]->line());
    if (m_questMark) paintQuestMark();
    resizeRangeBox();
    m_scene->setBackgroundBrush(QBrush(pixmap));
    setFinger(m_selNote);

}

void TfingerBoard::mouseMoveEvent(QMouseEvent *event) {
	if (m_isDisabled)
		return;
    int strNr = 7, fretNr = 99;
    if ( (event->y() >= fbRect.y()) && (event->y() <= (height()-fbRect.y()-4)) ) {
        int tx, ty = event->y();
        tx = mapToScene(event->x(), event->y()).x();
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
            paintFinger(m_workFinger, strNr, fretNr);
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

void TfingerBoard::mousePressEvent(QMouseEvent *event) {
	if (!m_isDisabled && event->button() == Qt::LeftButton) {
		if (m_curFret != 99 && m_curStr != 7) {
			m_selNote = posToNote(m_curStr,m_curFret);
			m_fingerPos = TfingerPos(m_curStr+1, m_curFret);
			if (gl->GpreferFlats)
				if (m_selNote.note != 3 && m_selNote.note != 7) // eliminate Fb from E and Cb from B
					m_selNote = m_selNote.showWithFlat();
			if (gl->GshowOtherPos)
				setFinger(m_selNote);
			else
				setFinger(TfingerPos(m_curStr+1, m_curFret));
			emit guitarClicked(m_selNote);
		} else {
			m_selNote = Tnote(0,0,0);
	//        setFinger(m_selNote);
		}
	}
}

Tnote TfingerBoard::posToNote(int str, int fret) {
    return Tnote(gl->Gtune()[str+1].getChromaticNrOfNote()+fret);
}

void TfingerBoard::setFinger(Tnote note) {
    if (note.note) {
        bool doShow = true;
        for(int i=0; i<6; i++) { // looking for pos to show
            int diff = note.getChromaticNrOfNote() - gl->Gtune()[gl->strOrder(i)+1].getChromaticNrOfNote();
            if ( doShow && diff >= 0 && diff <= gl->GfretsNumber) { // found
                if (diff == 0) { // open string
                    m_fingers[gl->strOrder(i)]->hide();
                    m_strings[gl->strOrder(i)]->show();
                } else { // some fret
                    m_strings[gl->strOrder(i)]->hide();
                    paintFinger(m_fingers[gl->strOrder(i)], gl->strOrder(i), diff);
                    m_fingers[gl->strOrder(i)]->show();
                }
                if (!gl->GshowOtherPos) {
                    doShow = false;
                }
            } else { // not found on this string or no need to show
                m_fingers[gl->strOrder(i)]->hide();
                m_strings[gl->strOrder(i)]->hide();
            }
        }
    } else {
        for (int i=0; i<6; i++) {
            m_fingers[i]->hide();
            m_strings[i]->hide();
        }
    }
    m_selNote = note;
}

void TfingerBoard::setFinger(TfingerPos pos) {
    for(int i=0; i<6; i++) {
        if (i != pos.str()-1) { //hide
            m_fingers[i]->hide();
            m_strings[i]->hide();
        }
        else { //show
            if (pos.fret()) { // some fret
                paintFinger(m_fingers[i], i, pos.fret());
                m_fingers[i]->show();
            }
            else { // open string
                m_fingers[i]->hide();
                m_strings[i]->show();
            }
        }
    }
}

void TfingerBoard::acceptSettings() {
    for (int i=0; i<6; i++) {
        m_fingers[i]->setPen(QPen(gl->GselectedColor));
        m_fingers[i]->setBrush(QBrush(gl->GselectedColor, Qt::SolidPattern));
    }
    m_workFinger->setBrush(QBrush(gl->GfingerColor, Qt::SolidPattern));
    paint();
    setFinger(m_selNote);
}

void TfingerBoard::paintFinger(QGraphicsEllipseItem *f, char strNr, char fretNr) {
    f->setPos(fretsPos[fretNr-1] - qRound(fretWidth/1.5),
              fbRect.y() + strGap*strNr + strGap/5);
}

void TfingerBoard::askQuestion(TfingerPos pos) {
    m_questPos = pos;
    QColor qC = gl->EquestionColor;
    qC.setAlpha(200); //it is too much opaque
    if (pos.fret()) { // some fret
        if (!m_questFinger) {
            m_questFinger = new QGraphicsEllipseItem();
            m_questFinger->setPen(QPen(qC));
            m_questFinger->setBrush(QBrush(qC, Qt::SolidPattern));
            m_scene->addItem(m_questFinger);
            m_questFinger->setRect(0,0, fretWidth/1.6, qRound(0.7*strGap));
            paintFinger(m_questFinger, pos.str()-1, pos.fret());
        }
    } else { // open strring
        if (!m_questString) {
            m_questString = new QGraphicsLineItem();
            m_questString->setPen(QPen(qC, m_strings[pos.str()-1]->pen().width(),
                                       Qt::SolidLine));
            m_scene->addItem(m_questString);
            m_questString->setLine(m_strings[pos.str()-1]->line());
        }
    }
    paintQuestMark();
}

void TfingerBoard::paintQuestMark() {
    if (!m_questMark) {
        QColor qC = gl->EquestionColor;
        qC.setAlpha(200); //it is too much opaque
        m_questMark = new QGraphicsSimpleTextItem();
        m_questMark->setBrush(QBrush(qC));
        m_scene->addItem(m_questMark);
        m_questMark->setText("?");
    }
#if defined(Q_OS_MACX)
    QFont f = QFont("nootka", 4*strGap, QFont::Normal);
#else
    QFont f = QFont("nootka", 2*strGap, QFont::Normal);
#endif
    m_questMark->setFont(f);
    int off = -1, off2 = 0;
    if (!gl->GisRightHanded) {
        m_questMark->scale(-1, 1);
        if (m_questPos.fret() == 1)
            off = 1;
        else
            off2 = fretWidth/2;
    }
    if (m_questPos.fret())
        m_questMark->setPos(fretsPos[m_questPos.fret() + off] - off2, (m_questPos.str()-1)*strGap );
    else {
        if (m_questPos.str() < 4)
            m_questMark->setPos(lastFret + fretWidth, (m_questPos.str()+1)*strGap);
        else
            m_questMark->setPos(lastFret + fretWidth, (m_questPos.str()-2)*strGap - strGap/2);
    }
}

void TfingerBoard::clearFingerBoard() {
    if (m_questFinger) {
        delete m_questFinger;
        m_questFinger = 0;
    }
    if (m_questString) {
        delete m_questString;
        m_questString = 0;
    }
    if (m_questMark) {
        delete m_questMark;
        m_questMark = 0;
    }
    if (m_rangeBox1)
        m_rangeBox1->hide();
    if (m_rangeBox2)
        m_rangeBox2->hide();
    setFinger(Tnote(0,0,0));
}

bool TfingerBoard::event(QEvent *event) {
    if (event->type() == QEvent::Leave) {
        if (m_curStr != 7) m_workStrings[m_curStr]->hide();
        m_curStr = 7;
        m_workFinger->hide();
        m_curFret = 99;
    }
    return QGraphicsView::event(event);
}

void TfingerBoard::createRangeBox(char loFret, char hiFret) {
    m_loFret = loFret;
    m_hiFret = hiFret;
    if (!m_rangeBox1) {
        m_rangeBox1 = new QGraphicsRectItem();
        m_scene->addItem(m_rangeBox1);
        m_rangeBox1->setBrush(QBrush(Qt::NoBrush));
    }
    if (!m_rangeBox2 && m_loFret == 0 && m_hiFret > 0 && m_hiFret < gl->GfretsNumber) {
        m_rangeBox2 = new QGraphicsRectItem();
        m_scene->addItem(m_rangeBox2);
        m_rangeBox2->setBrush(QBrush(Qt::NoBrush));
    }
    resizeRangeBox();
}

void TfingerBoard::resizeRangeBox() {
    if (m_rangeBox1) {
        QColor C = gl->EanswerColor;
        C.setAlpha(200);
        QPen pen = QPen(C, strGap/3);
	pen.setJoinStyle(Qt::RoundJoin);
        int xxB, xxE;
        if (m_loFret == 0 || m_loFret == 1)
            xxB = xxB = fbRect.x() - 4;
        else
            xxB = fretsPos[m_loFret-2] - 4;
        if (m_loFret == 0) {//  surely box for open strings
            if (m_hiFret == 0) { // one box over hole
                xxB = lastFret + strGap;
                xxE = width() - strGap;
            }
            else if (m_hiFret < gl->GfretsNumber) { // both, one over hole
                m_rangeBox2->setPen(pen);
                m_rangeBox2->setRect(0, 0, width() - lastFret - 2* strGap, fbRect.height() + 8);
                m_rangeBox2->setPos(lastFret + strGap , fbRect.y() - 4);
                xxE = fretsPos[m_hiFret-1] + 4;
            } else { // one - over whole guitar
                xxE = width() - strGap;
            }
        } else { //one
            xxE = fretsPos[m_hiFret-1] + 4;
        }
        m_rangeBox1->setPen(pen);
        m_rangeBox1->setRect(0, 0, xxE - xxB, fbRect.height() + 8);
        m_rangeBox1->setPos(xxB, fbRect.y() - 4);
    }
}

void TfingerBoard::prepareAnswer() {
    if (m_rangeBox1)
        m_rangeBox1->show();
    if (m_rangeBox2)
        m_rangeBox2->show();
}

void TfingerBoard::deleteRangeBox() {
    if (m_rangeBox1) {
        delete m_rangeBox1;
        m_rangeBox1 = 0;
    }
    if (m_rangeBox2) {
        delete m_rangeBox2;
        m_rangeBox2 = 0;
    }
}

void TfingerBoard::setGuitarDisabled(bool disabled) {
	if (disabled) {
		setMouseTracking(false);
		m_isDisabled = true;
		m_workFinger->hide();
		for(int i = 0; i<6; i++)
			m_workStrings[i]->hide();
	} else {
		setMouseTracking(true);
		m_isDisabled = false;
	}
}
