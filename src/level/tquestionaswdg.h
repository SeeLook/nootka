/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TQUESTIONASWDG_H
#define TQUESTIONASWDG_H

#include "tqatype.h"
#include <QObject>
#include <QCheckBox>

class QGridLayout;
class QLabel;


/** Widget aumatically inserting into QGridLayout object
 * to select types of answers for question. */
class TquestionAsWdg : public QObject
{
    Q_OBJECT
public:
    explicit TquestionAsWdg(QString title, QGridLayout *gridLay, int row, QWidget *parent = 0);

    static QString questionTxt() { return tr("question"); } // question
    static QString questionsTxt() { return tr("questions"); } // questions
    static QString answerTxt() { return tr("answer"); } // answer
    static QString answersTxt() { return tr("answers"); } // answers
    static QString asNoteTxt() { return tr("as note in the score"); } // as note in the score
    static QString asNameTxt() { return tr("as note's name"); } // as note's name
    static QString asFretPosTxt() { return tr("as position on the fingerboard"); } // as position on the fingerboard
    static QString asSoundTxt() { return tr("as played sound"); } // as played sound

    bool answerAsNote() { return asNoteChB->isChecked(); }
    bool answerAsName() { return asNameChB->isChecked(); }
    bool answerAsPos() { return asFretPosChB->isChecked(); }
    bool answerAsSound() { return asSoundChB->isChecked(); }
    
    void setChecked(bool checked) { enableChBox->setChecked(checked); }
    bool isChecked() { return enableChBox->isChecked(); }
    void setTitle(QString title); // Text of left label
    void setQuestionTip(QString tip); // First line of Text in statusTip Message
    

    void setAnswers(TQAtype types);
    TQAtype getAnswers();

    QCheckBox *enableChBox, *asNoteChB, *asNameChB, *asFretPosChB, *asSoundChB;
    QLabel *questLab;

signals:
        /** This signal is emited when any QCheckBox changes his state. */
    void answerStateChanged();

private slots:
    void buttonClicked();
    void groupChecked();

};

#endif // TQUESTIONASWDG_H
