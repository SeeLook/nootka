#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>
#include "tnote.h"
#include "ttune.h"
#include "tscorewidgetsimple.h"




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

class GlobalSettings : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalSettings(QWidget *parent = 0);

    void saveSettings();
private:
    QCheckBox *otherEnharmChBox, *dblAccChBox;
};



class ScoreSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ScoreSettings(QWidget *parent = 0);

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
    Tnote::Enotation m_workStyle;


};


class NameSettings : public QWidget
{
    Q_OBJECT
public:
    explicit NameSettings(QWidget *parent = 0);

    void saveSettings();

private:
    TnotationRadioGroup *nameStyleGr;
    QCheckBox *octInNameCh;
};

//##############################################################
class GuitarSettings : public QWidget
{
    Q_OBJECT
public:
    explicit GuitarSettings(QWidget *parent = 0);

    void saveSettings();

private:
    void setTune (Ttune tune);

    TscoreWidgetSimple *tuneView;
    QCheckBox *righthandCh, *morePosCh;
    QRadioButton *prefFlatBut, *prefSharpBut;
    QSpinBox *fretsNrSpin;
    QComboBox *tuneCombo;
};

//##############################################################
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

public slots:
    void saveSettings();

private:
    QListWidget *navList;
    QStackedLayout *stackLayout;
    QPushButton *cancelBut, *okBut;

    GlobalSettings *m_globalSett;
    ScoreSettings *m_scoreSett;
    NameSettings *m_nameSett;
    GuitarSettings *m_guitarSett;

};

#endif // SETTINGSDIALOG_H
