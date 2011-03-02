#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//#include <QDialog>
//#include <QButtonGroup>
//#include <QRadioButton>
#include <QtGui>
#include "tnote.h"



/**
@author Tomasz Bojczuk
*/
class TnotationRadioGroup: public QGroupBox
{
        Q_OBJECT
public:
        TnotationRadioGroup(Tnote::Enotation _notation, QWidget *parent = 0);
        ~TnotationRadioGroup();

        Tnote::Enotation notation;

        static const QString strNorsk;
        static const QString strItal;
        static const QString strDeutsch;
        static const QString strEnglish;
        static const QString strNeder;
        static const QString strNorskExampl;
        static const QString strItalExampl;
        static const QString strDeutschExampl;
        static const QString strEnglishExampl;
        static const QString strNederExampl;

        Tnote::Enotation getNameStyle();

signals:
        void noteNameStyleWasChanged (Tnote::Enotation);

public slots:
        void noteNameStyleWasClicked();

private:
        QRadioButton *norskButt;
        QRadioButton *deutschButt;
        QRadioButton *italianoButt;
        QRadioButton *englishButt;
        QRadioButton *nederlButt;
        QButtonGroup *buttonGroup;

};

class EnharmAndDblAccChBox : public QWidget
{
    Q_OBJECT
public:
    explicit EnharmAndDblAccChBox(bool otherEnharm, bool dblAcc, QWidget *parent = 0);
    inline bool isEnableDblAccid();
    inline bool isEnableOtherEnharmNotes();

private:
    QCheckBox *otherEnharmChBox, *dblAccChBox;
};



class ScoreSettingsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit ScoreSettingsDlg(QWidget *parent = 0);

    static const QString forExample;
    static const QString showKeySigName;

    QString getMajorExample(Tnote::Enotation nameStyle);
    QString getMinorExample(Tnote::Enotation nameStyle);

signals:

public slots:
    void enableKeySignGroup(bool enable);
    void nameStyleWasChanged(Tnote::Enotation nameStyle);
    void majorExtensionChanged();
    void minorExtensionChanged();
    void saveSettings();

private:
    QCheckBox *enablKeySignCh;
    QGroupBox *enablKeyNameGr, *nameExtGr;
    QLabel *majExtLab, *minExtLab, *majExampl, *minExampl;
    QLineEdit *majEdit, *minEdit;
    TnotationRadioGroup *nameStyleGr;
    EnharmAndDblAccChBox *enhAndDblAcc;
    QPushButton *cancelBut, *okBut;
    Tnote::Enotation m_workStyle;


};



#endif // SETTINGSDIALOG_H
