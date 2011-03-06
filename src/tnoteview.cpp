#include "tnoteview.h"
#include "tscorewidgetsimple.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsLineItem>

#include <iostream>


TnoteView::TnoteView(int _index, TscoreWidgetSimple * parent) :
    QGraphicsView(parent)
{
    m_index = _index;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(("background: transparent"));
    setRenderHint(QPainter::Antialiasing, true);
    setMouseTracking(true);

    m_scene = new QGraphicsScene();
    setScene(m_scene);

    m_workColor = palette().highlight().color();
    m_workColor.setAlpha(150);
    m_mainColor = palette().windowText().color();

    for (int i=0; i<7; i++) {
        m_upLines[i] = new QGraphicsLineItem();
        m_upLines[i]->hide();
        m_upLines[i]->setPen(QPen(m_workColor));
        m_scene->addItem(m_upLines[i]);

        m_mainUpLines[i] = new QGraphicsLineItem();
        m_mainUpLines[i]->hide();
        m_scene->addItem(m_mainUpLines[i]);
        if (i < 5) {
            m_downLines[i] = new QGraphicsLineItem();
            m_downLines[i]->hide();
            m_downLines[i]->setPen(QPen(m_workColor));
            m_scene->addItem(m_downLines[i]);

            m_mainDownLines[i] = new QGraphicsLineItem();
            m_mainDownLines[i]->hide();
            m_scene->addItem(m_mainDownLines[i]);
        }
    }

    m_mainNote = new QGraphicsEllipseItem();
    m_mainNote->hide();
    m_scene->addItem(m_mainNote);

    m_workNote = new QGraphicsEllipseItem();
    m_workNote->hide();
    m_workNote->setPen(QPen(m_workColor));
    m_workNote->setBrush(QBrush(m_workColor,Qt::SolidPattern));
    m_scene->addItem(m_workNote);

    m_mainAccid = new QGraphicsSimpleTextItem();
    m_scene->addItem(m_mainAccid);
    m_workAccid = new QGraphicsSimpleTextItem();
    m_workAccid->setBrush(QBrush(m_workColor));
    m_scene->addItem(m_workAccid);
    m_workAccid->hide();

    setColor(m_mainColor);
    m_accInKeyPtr = parent->accInKeyArr;
    m_dblAccFusePtr = &parent->dblAccidFuse;
    m_curentAccid = 0;
    m_accidental = 0;
    m_workPosY = 0;
    m_mainPosY = 0;
    m_ambitMin = 31; // a lowest note on a standard guitar tune
    m_ambitMax = 5; // a highest one
//    setDblAccFuse(1);

}


void TnoteView::resize(int co) {
    m_coeff = co;
    m_scene->setSceneRect(0,0,6*m_coeff,height());
    m_workNote->setRect(0,0,m_coeff*3,m_coeff*2-2);
    m_mainNote->setRect(0,0,m_coeff*3,m_coeff*2-2);
    for (int i=0; i<7; i++) {
        m_upLines[i]->setLine((qreal)2*m_coeff,(qreal)m_coeff*2*(i+1),(qreal)width(),(qreal)m_coeff*2*(i+1));
        m_mainUpLines[i]->setLine((qreal)2*m_coeff,(qreal)m_coeff*2*(i+1),(qreal)width(),(qreal)m_coeff*2*(i+1));
        if (i < 5) {
            m_downLines[i]->setLine((qreal)2*m_coeff,(qreal)m_coeff*2*(i+13),(qreal)width(),(qreal)m_coeff*2*(i+13));
            m_mainDownLines[i]->setLine((qreal)2*m_coeff,(qreal)m_coeff*2*(i+13),(qreal)width(),(qreal)m_coeff*2*(i+13));
        }
    }
    if (m_mainNote->isVisible() && m_mainPosY) moveNote(m_mainPosY);

    qreal fontFactor = 3.6;
    QFont font(QFont("Emmentaler"));
    font.setPointSizeF(fontFactor*m_coeff);
    m_workAccid->setFont(font);
    m_mainAccid->setFont(font);
    bool reset = false;
    if (m_workAccid->text() == "") {
        m_workAccid->setText(QString(QChar(0xe11a)));
        reset = true;
    }
    m_accTextOffset = m_workAccid->boundingRect().height()/2;
    if (reset) m_workAccid->setText("");



}

void TnoteView::mouseMoveEvent(QMouseEvent * event) {
    if (
            (event->x() > 2) && (event->x() < 5*m_coeff) &&
        (event->y() > m_ambitMax*m_coeff) && (event->y() < m_ambitMin*m_coeff) ) {
        int a = event->y()/m_coeff;
        if (m_workPosY != a) {
            m_workPosY = a;
            m_workNote->setPos(2.5*m_coeff,m_workPosY*m_coeff+1);
            m_workAccid->setPos(1,(m_workPosY+1)*m_coeff-m_accTextOffset);

            for (int i=0; i < 7; i++)	{
                if (m_workPosY < (2*(i+1)))
                    m_upLines[i]->show();
                else m_upLines[i]->hide();
            }
            for (int i=0; i < 5; i++)	{
                if (m_workPosY > (2*(i+12)))
                    m_downLines[i]->show();
              else m_downLines[i]->hide();
            }

            if (!m_workNote->isVisible()) {
                m_workAccid->show();
                m_workNote->show();
            }
       }
    }
    else
        hideWorkNote();
}

void TnoteView::mousePressEvent(QMouseEvent * event) {
    if (event->button() == Qt::LeftButton && m_workPosY) {
        m_mainPosY = m_workPosY;
        m_accidental = m_curentAccid;
        moveNote(m_mainPosY);
        emit noteWasClicked(m_index);
    }
}

void TnoteView::wheelEvent(QWheelEvent * event) {
    int prevAcc = m_curentAccid;
    if (event->delta() < 0) {
        m_curentAccid--;
        if (m_curentAccid < -*(m_dblAccFusePtr)) m_curentAccid = -*(m_dblAccFusePtr);
    } else {
        m_curentAccid++;
        if (m_curentAccid > *(m_dblAccFusePtr)) m_curentAccid = *(m_dblAccFusePtr);
    }
    if (prevAcc != m_curentAccid) {
        m_workAccid->setText(getAccid(m_curentAccid));
        emit accidWasChanged(m_curentAccid);
    }
}

void TnoteView::moveNote(int pos) {
    m_mainNote->setPos(2.5*m_coeff,pos*m_coeff+1);
//    m_mainAccid->setPos(1,(pos-accidTextOffset)*m_coeff);
    m_mainAccid->setPos(1,(pos+1)*m_coeff-m_accTextOffset);
    if (*(m_accInKeyPtr+(39-pos)%7)) {
      if ( m_accidental == 0 ) m_mainAccid->setText(getAccid(3));
      else
        if (*(m_accInKeyPtr+(39-pos)%7) == m_accidental ) m_mainAccid->setText(getAccid(0));
        else
            m_mainAccid->setText(getAccid(m_accidental));
    } 
    else
        m_mainAccid->setText(getAccid(m_accidental));
    if (!m_mainNote->isVisible()) {
        m_mainNote->show();
        m_mainAccid->show();
    }

    for (int i=0; i < 7; i++)	{
        if (pos < (2*(i+1)))
            m_mainUpLines[i]->show();
        else m_mainUpLines[i]->hide();
    }
    for (int i=0; i < 5; i++)	{
        if (pos > (2*(i+12)))
            m_mainDownLines[i]->show();
      else m_mainDownLines[i]->hide();
    }
}

QString TnoteView::getAccid(int accNr) {
    const int accCharTable[6] = { 0xe123, 0xe11a, 0x20, 0xe10e, 0xe125, 0xe116 };
    QString str = QString(QChar(accCharTable[accNr+2]));
    return str;
}

void TnoteView::setAccidText(int accNr) {
    m_workAccid->setText(getAccid(accNr));
    m_curentAccid = accNr;

}

void TnoteView::setNote(int notePos, char accNr) {
    m_mainPosY = notePos;
    m_accidental = accNr;
    moveNote(notePos);
    setAccidText(accNr);
}

void TnoteView::hideNote() {
    m_mainNote->hide();
    m_mainAccid->hide();
    for (int i=0; i < 7; i++)	{
        m_mainUpLines[i]->hide();
    }
    for (int i=0; i < 5; i++)	{
        m_mainDownLines[i]->hide();
    }
}

void TnoteView::setColor(QColor color) {
    m_mainColor = color;
    m_mainNote->setPen(QPen(m_mainColor));
    m_mainNote->setBrush(QBrush(m_mainColor,Qt::SolidPattern));
    m_mainAccid->setBrush(QBrush(m_mainColor));
    for (int i=0; i<7; i++) {
        m_mainUpLines[i]->setPen(QPen(color));
        if (i < 5) m_mainDownLines[i]->setPen(QPen(color));
    }
}

//bool TnoteView::event(QEvent *event) {
//    if (event->type() == QEvent::Leave) {
//        hideWorkNote();
//        std::cout << "kuku\n";
//        return true;
//    }
//    return QWidget::event(event);
//}

void TnoteView::hideWorkNote() {
    m_workNote->hide();
    m_workAccid->hide();
    for (int i=0; i < 7; i++) m_upLines[i]->hide();
    for (int i=0; i < 5; i++) m_downLines[i]->hide();
    m_workPosY = 0;
}
