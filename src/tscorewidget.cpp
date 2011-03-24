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

#include "tscorewidget.h"
#include "tnoteview.h"
#include "tglobals.h"
#include <QDebug>


extern Tglobals *gl;

TscoreWidget::TscoreWidget(unsigned char _notesCount, QWidget *parent) :
    TscoreWidgetSimple(_notesCount, parent)
{


    noteViews[0]->setStatusTip(tr("Click to sellect a note, use mouse wheel to change accidentals."));

    if (gl->enharmNotesColor == -1)
        gl->enharmNotesColor = palette().highlight().color();

    noteViews[1]->setDisabled(true);
    noteViews[1]->setColor(gl->enharmNotesColor);
    noteViews[2]->setDisabled(true);
    noteViews[2]->setColor(gl->enharmNotesColor);

    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    setEnableKeySign(gl->keySignatureEnabled);

    setAmbitus(Tnote(gl->Gtune.lowest().getChromaticNrOfNote()-1),
               Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber+1));

    connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
}


void TscoreWidget::whenNoteWasChanged(int index, Tnote note) {
    //We are sure that index is 0, cause others are disabled :-)
    if (gl->showEnharmNotes) {
        TnotesList enharmList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        TnotesList::iterator it = enharmList.begin();
        ++it;
        if (it != enharmList.end())
            setNote(1,*(it));
        else
            clearNote(1);
        if (gl->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end())
                setNote(2,*(it));
            else
                clearNote(2);
        }
    }
    emit noteChanged(index, note);
}

void TscoreWidget::setEnableEnharmNotes(bool isEnabled) {
    if (!isEnabled) {
        clearNote(1);
        clearNote(2);
    }
}

void TscoreWidget::paintEvent(QPaintEvent *event) {
    TscoreWidgetSimple::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0,0,width(),height());

    painter.setPen(QPen(palette().foreground().color()));
////    painter.setBrush(QBrush(palette().base().color(),Qt::SolidPattern));
    QFont f = painter.font();
    f.setPixelSize(qRound(1.5*coeff));
    painter.setFont(f);
    paintScord(painter, QPointF(10,29*coeff),6,Tnote(3,0,-1));
//    painter.drawText(10, 31*coeff,"6 = D");
//    painter.drawEllipse(7, 29*coeff,
//                        qRound(painter.font().pointSize()*1.5),
//                        qRound(painter.font().pointSize()*1.5));

}

void TscoreWidget::paintScord(QPainter &p, QPointF off, int str, Tnote n) {
    QString S = "";
    if (n.note)
        S = " = " + QString::fromStdString(n.getName(gl->NnameStyleInNoteName,false));
    p.drawText(QRectF(off.x(), off.y(), coeff*10, coeff*1.5), Qt::AlignLeft,
               QString("%1").arg(str) + S);
    p.drawEllipse(QPointF(off.x()+coeff*0.75, off.y()+coeff*0.75),coeff*0.8,coeff*0.8);
}

//void TscoreWidget::contextMenuEvent(QContextMenuEvent *event) {
//    if (gl->keySignatureEnabled)
//        contextMenu->addAction(keySigNameAct);
//    else contextMenu->removeAction(keySigNameAct);
//    contextMenu->addAction(scoreSettAct);
//    contextMenu->exec(event->globalPos());

//}

//void TscoreWidget::createActions() {
//    keySigNameAct = new QAction(ScoreSettingsDlg::showKeySigName,this);
//    keySigNameAct->setCheckable(true);
//    keySigNameAct->setChecked(gl->showKeySignName);
//    connect(keySigNameAct, SIGNAL(triggered(bool)), this, SLOT(enableKeySigNameSlot(bool)));

//    scoreSettAct = new QAction(tr("score settings"),this);
//    connect(scoreSettAct, SIGNAL(triggered()), this, SLOT(createScoreSettingsDialog()));
//}


//void TscoreWidget::createScoreSettingsDialog() {

//    ScoreSettingsDlg *scoreSettDlg = new ScoreSettingsDlg();
//    if (scoreSettDlg->exec() == QDialog::Accepted) {
//        keySigNameAct->setChecked(gl->showKeySignName);
//        enableDblAccidsSlot(gl->doubleAccidentalsEnabled);
//        emit enableDblAccWasChanged(gl->doubleAccidentalsEnabled);
//        setEnableKeySign(gl->keySignatureEnabled);
//        if (gl->keySignatureEnabled) refreshKeySignNameStyle();
//    }
//}

//void TscoreWidget::enableDblAccidsSlot(bool isEnabled) {
//    setEnabledDblAccid(isEnabled);
//    gl->doubleAccidentalsEnabled = isEnabled;
//}

//void TscoreWidget::enableKeySigNameSlot(bool isEnabled) {
//    gl->showKeySignName = isEnabled;
//    refreshKeySignNameStyle();
//}
