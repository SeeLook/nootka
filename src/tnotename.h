#ifndef TNOTENAME_H
#define TNOTENAME_H

#include <QWidget>
#include "tnote.h"

class QLabel;
class QPushButton;
class QButtonGroup;

class TnoteName : public QWidget
{
    Q_OBJECT
public:
    explicit TnoteName(QWidget *parent = 0);

    static const QString octaves[6];

    void setNoteNamesOnButt(Tnote::Enotation nameStyle);
//    void setButtonsWithOctaves

signals:

public slots:

private:
    QLabel *nameLabel;
    QPushButton *noteButtons[7];
    QPushButton *octaveButtons[6];
    QPushButton *dblFlatButt, *flatButt, *sharpButt, *dblSharpButt;
    QButtonGroup *noteGroup, *octaveGroup;

};

#endif // TNOTENAME_H
