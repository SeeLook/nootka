/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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

#include <nootkacoreglobal.h>
#include <exam/tqatype.h>
#include <QObject>
#include <QCheckBox>

class QGridLayout;
class QLabel;


/** 
 * Widget automatically inserting into QGridLayout object
 * to select types of answers for question. 
 */
class NOOTKACORE_EXPORT TquestionAsWdg : public QObject
{
    Q_OBJECT
public:
    explicit TquestionAsWdg(TQAtype::Etype type, QGridLayout *gridLay, int row, QWidget *parent = 0);

    static QString questionTxt() { return tr("question"); } // question
    static QString questionsTxt() { return tr("questions"); } // questions
    static QString answerTxt() { return tr("answer"); } // answer
    static QString answersTxt() { return tr("answers"); } // answers
    static QString asNoteTxt() { return tr("as note on the staff"); } // as note on the staff
    static QString asNameTxt() { return tr("as note name"); } // as note's name
    static QString asFretPosTxt() { return tr("as position on the fingerboard"); } // as position on the fingerboard
    static QString asSoundTxt() { return tr("as played sound"); } // as played sound
        /** Returns string with kind of question/answer text. */
    static QString qaTypeText(TQAtype::Etype type);
        /** Returns a letter in nootka font with symbol represents type. */
    static QString qaTypeSymbol(TQAtype::Etype type);    

    bool answerAsNote() { return asNoteChB->isChecked(); }
    bool answerAsName() { return asNameChB->isChecked(); }
    bool answerAsPos() { return asFretPosChB->isChecked(); }
    bool answerAsSound() { return asSoundChB->isChecked(); }
    
    void setChecked(bool checked);
    bool isChecked() { return enableChBox->isChecked(); }
    void setTitle(QString title); // Text of left label
    

    void setAnswers(TQAtype types);
    TQAtype getAnswers();

    QCheckBox *enableChBox, *asNoteChB, *asNameChB, *asFretPosChB, *asSoundChB;
    QLabel *questLab;

signals:
        /** This signal is emitted when any QCheckBox changes his state. */
    void answerStateChanged();

private slots:
    void buttonClicked();
    void groupChecked();

};

#endif // TQUESTIONASWDG_H
