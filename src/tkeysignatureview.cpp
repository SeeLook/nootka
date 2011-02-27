#include "tkeysignatureview.h"
#include "tscorewidgetsimple.h"
#include "tglobals.h"
#include <cstdlib>


#include <iostream>

//const qreal accidTextOffset = 6.8;


extern Tglobals *gl;

/*static*/
char TkeySignatureView::majorKeySignatures[15][2] = { {1,-1},{5,-1},{2,-1},{6,-1},{3,-1},
                {7,-1},{4,0},{1,0},{5,0},{2,0},{6,0},{3,0},{7,0},{4,1},{1,1} };
char TkeySignatureView::minorKeySignatures[15][2] = { {6,-1},{3,-1},{7,-1},{4,0},{1,0},{5,0},{2,0},{6,0},{3,0},{7,0},{4,1},{1,1},{5,1},{2,1},{6,1} };


TkeySignatureView::TkeySignatureView(TscoreWidgetSimple *parent, char _keySign) :
    QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(("background: transparent"));
    setRenderHint(QPainter::Antialiasing, true);
    setMouseTracking(true);
    setStatusTip(tr("Use mouse wheel to change key signature"));

    m_scene = new QGraphicsScene();
    setScene(m_scene);

    m_posOfAccid[0] = 16; // Fes & Fis (F#)
    m_posOfAccid[1] = 19; // Ces (C#)
    m_posOfAccid[2] = 15; // Ges (G#)
    m_posOfAccid[3] = 18; // Des (D#)
    m_posOfAccid[4] = 21; // As  (A#)
    m_posOfAccid[5] = 17; // ES  (E#)
    m_posOfAccid[6] = 20; // B   (H#)  (Bb - B#) in west

    for (int i=0; i<7; i++) {
        m_accidentals[i] = new QGraphicsSimpleTextItem();
        m_accidentals[i]->setBrush(QBrush(palette().text().color()));
        m_scene->addItem(m_accidentals[i]);
        m_accidentals[i]->hide();
    }
//    m_accidentals[0]->setText(QString(QChar(0xe11a)));

    m_keyNameText = new QGraphicsSimpleTextItem();
    m_scene->addItem(m_keyNameText);

    m_accInKeyPtr = parent->accInKeyArr;
    m_keySignature = _keySign;
    setNameStyle();
    setKeySignature(_keySign);
    show();

}

TkeySignatureView::~TkeySignatureView() {
}

void TkeySignatureView::resize(int co) {
    m_coeff = co;
    m_scene->setSceneRect(0,0,8*m_coeff,height());
    const qreal fontFactor = 3.5;
    QFont font(QFont("Emmentaler"));
    font.setPointSizeF(fontFactor*m_coeff);
    int base;
    if (m_accidentals[0]->text() == "")
        m_accidentals[0]->setText(QString(QChar(0xe11a)));
    m_accidentals[0]->setFont(font);
    m_accTextOffset = m_accidentals[0]->boundingRect().height()/2-m_coeff/5;
    if (m_keySignature >= 0) base = 0;
    else base = 6;
    for (int i=0; i<7; i++) {
        m_accidentals[i]->setFont(font);
        m_accidentals[i]->setPos(i*m_coeff+1,
                    m_posOfAccid[std::abs(base-i)]*m_coeff-m_accTextOffset);
    }
    m_keyNameText->setPos(1,10*m_coeff);

}

void TkeySignatureView::wheelEvent(QWheelEvent * event) {
    char prevKey = m_keySignature;
    int base;
    if (event->delta() > 0) {
        if (m_keySignature < 7) m_keySignature++;
    }
    else {
        if (m_keySignature > -7) m_keySignature--;
    }
    if (m_keySignature != prevKey) {
        char sign;
        if (m_keySignature > 0) { //sharp
            m_accidentals[std::abs(m_keySignature)-1]->setText(QString(QChar(0xe10e)));
            sign = 1;
            base = 0;
        }
        else
            if (m_keySignature < 0) { //flat
                m_accidentals[std::abs(m_keySignature)-1]->setText(QString(QChar(0xe11a)));
            sign = -1;
            base = 8;
        }

        if (std::abs(m_keySignature) > (std::abs(prevKey))) {
            m_accidentals[std::abs(m_keySignature)-1]->setPos(
                    (std::abs(m_keySignature)-1)*m_coeff+1,
                    qreal(m_posOfAccid[std::abs(base-std::abs(m_keySignature))-1]*m_coeff)-m_accTextOffset);
            m_accidentals[std::abs(m_keySignature)-1]->show();
            *(m_accInKeyPtr+(26-m_posOfAccid[std::abs(base-std::abs(m_keySignature))-1])%7) = sign;
        }
        else {
            m_accidentals[std::abs(prevKey)-1]->hide();
            if (m_keySignature == 0) {
                if (prevKey == -1) base = 8;
                    else base = 0;
                }
            *(m_accInKeyPtr+(26-m_posOfAccid[std::abs(base-std::abs(prevKey))-1])%7) = 0;
        }
        if (gl->showKeySignName)
            m_keyNameText->setText(majorKeysNames[m_keySignature+7]+"\n" + minorKeysNames[m_keySignature+7]);
        emit keySignWasChanged();

    }

}

void TkeySignatureView::setKeySignature(char keySign) {
    m_keySignature = keySign;
    if (gl->showKeySignName)
        m_keyNameText->setText(majorKeysNames[m_keySignature+7]+"\n" + minorKeysNames[m_keySignature+7]);

}

void TkeySignatureView::setNameStyle() {
    if (gl->majKeyNameSufix == " ") gl->majKeyNameSufix = tr("major");
    if (gl->minKeyNameSufix == " ") gl->minKeyNameSufix = tr("minor");
    Tnote someNote;
    for (int i=0; i<15; i++) {
        someNote = Tnote(majorKeySignatures[i][0],0,majorKeySignatures[i][1]);
        majorKeysNames[i] = QString::fromStdString((someNote.getName(gl->nameStyleInKeySign,false)));
        if (gl->majKeyNameSufix != "") majorKeysNames[i] += "-" + gl->majKeyNameSufix;
        someNote = Tnote(minorKeySignatures[i][0],0,minorKeySignatures[i][1]);
        minorKeysNames[i] = QString::fromStdString(someNote.getName(gl->nameStyleInKeySign,false)).toLower();
        if (gl->minKeyNameSufix != "") minorKeysNames[i] += "-" + gl->minKeyNameSufix;
    setKeySignature(m_keySignature);
    }
    if (gl->showKeySignName) m_keyNameText->show();
    else m_keyNameText->hide();
}

//m_flatKeyFuse = -7;
//m_sharpKeyFuse = 7;
