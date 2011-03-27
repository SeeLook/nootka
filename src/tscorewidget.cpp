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
  if (gl->Gtune != Ttune::stdTune) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0,0,width(),height());
    painter.setPen(QPen(palette().foreground().color()));
//    I HATE THIS PART... BUT IT WORKS
    /** @todo Its size has to depends on @param coeff */
    QFont f = QFont("Arial");
    f.setPixelSize(11);
    painter.setFont(f);
    Ttune sT = Ttune::stdTune;
    int nL = 0;
    for (int i=1; i<7; i++) {
        if ( gl->Gtune[i] != sT[i])
            nL++;
    }
    int xOffBase = coeff;
    if (nL>3) xOffBase = 0;
    int xOff = xOffBase;
    int yOff = -1;
    int c = 0;
    for (int i=1; i<7; i++) {
      if ( gl->Gtune[i] != sT[i]) {
        if ( nL>3 && c%2 == 1 ) {
            xOff = 45;
        } else {
            yOff++;
            xOff = xOffBase;
        }
        c++;
//        paintScord(painter, QPointF(5+xOff,29*coeff+17*yOff),i,gl->Gtune[i]);
        int fa =15;
        painter.drawText(QRectF(5+xOff,29*coeff+17*yOff, fa, fa), Qt::AlignCenter,
               QString("%1").arg(i));
        painter.drawText(QRectF(5+xOff+fa, 29*coeff+17*yOff, 60, fa),Qt::AlignLeft,
             " =" + QString::fromStdString(gl->Gtune[i].getName(gl->NnameStyleInNoteName,false)));
        painter.drawEllipse(5+xOff,29*coeff+17*yOff,fa,fa);
      }
    }
  }
}

void TscoreWidget::acceptSettings() {
    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    if (gl->Gtune != Ttune::stdTune)
        setHasScord(true);
    else // resizing is done in setEnableKeySign() method;
        setHasScord(false);
    setEnableKeySign(gl->keySignatureEnabled);
    if (!gl->doubleAccidentalsEnabled) clearNote(2);
    setEnableEnharmNotes(gl->showEnharmNotes);
    if (gl->keySignatureEnabled) refreshKeySignNameStyle();
    setAmbitus(Tnote(gl->Gtune.lowest().getChromaticNrOfNote()-1),
               Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber+1));
//    repaint();
    update();
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
