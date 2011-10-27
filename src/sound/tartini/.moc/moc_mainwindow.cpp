/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Jun 21 21:45:39 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/mainwindow/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      35,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      40,   11,   11,   11, 0x05,
      56,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      73,   11,   11,   11, 0x0a,
      93,   84,   11,   11, 0x0a,
     115,   11,   11,   11, 0x0a,
     128,   11,   11,   11, 0x0a,
     153,  145,   11,   11, 0x0a,
     170,   11,   11,   11, 0x0a,
     194,  188,   11,   11, 0x0a,
     219,   11,   11,   11, 0x0a,
     237,   11,   11,   11, 0x0a,
     264,  261,   11,   11, 0x0a,
     289,   11,   11,   11, 0x0a,
     326,  319,  310,   11, 0x0a,
     342,  340,   11,   11, 0x0a,
     363,   11,   11,   11, 0x0a,
     379,   11,   11,   11, 0x0a,
     404,  394,   11,   11, 0x0a,
     432,   11,   11,   11, 0x0a,
     448,   11,   11,   11, 0x0a,
     465,   11,   11,   11, 0x0a,
     483,   11,   11,   11, 0x0a,
     504,   11,   11,   11, 0x0a,
     529,  526,   11,   11, 0x0a,
     548,   11,   11,   11, 0x0a,
     563,   11,   11,   11, 0x0a,
     574,   11,   11,   11, 0x0a,
     584,   11,   11,   11, 0x0a,
     604,   11,   11,   11, 0x0a,
     633,  617,   11,   11, 0x0a,
     660,   11,   11,   11, 0x0a,
     685,   11,   11,   11, 0x0a,
     712,   11,  707,   11, 0x0a,
     731,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0changedMusicFolder(QString)\0"
    "zoomInPressed()\0zoomOutPressed()\0"
    "openFile()\0filename\0openFile(const char*)\0"
    "openRecord()\0openPlayRecord()\0andPlay\0"
    "openRecord(bool)\0closeAllWidgets()\0"
    "event\0closeEvent(QCloseEvent*)\0"
    "menuPreferences()\0windowMenuAboutToShow()\0"
    "id\0windowMenuActivated(int)\0"
    "newViewAboutToShow()\0QWidget*\0viewID\0"
    "openView(int)\0t\0setTimeLabel(double)\0"
    "setChunkLabel()\0setNoteLabel()\0min_,max_\0"
    "setTimeRange(double,double)\0rewindPressed()\0"
    "rewindReleased()\0playStopClicked()\0"
    "fastforwardPressed()\0fastforwardReleased()\0"
    "ch\0setTitle(Channel*)\0aboutTartini()\0"
    "aboutGPL()\0aboutQt()\0showDocumentation()\0"
    "printPitch()\0type,typeString\0"
    "exportChannel(int,QString)\0"
    "exportChannelPlainText()\0exportChannelMatlab()\0"
    "bool\0loadViewGeometry()\0saveViewGeometry()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
	return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changedMusicFolder((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: zoomInPressed(); break;
        case 2: zoomOutPressed(); break;
        case 3: openFile(); break;
        case 4: openFile((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 5: openRecord(); break;
        case 6: openPlayRecord(); break;
        case 7: openRecord((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: closeAllWidgets(); break;
        case 9: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 10: menuPreferences(); break;
        case 11: windowMenuAboutToShow(); break;
        case 12: windowMenuActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: newViewAboutToShow(); break;
        case 14: { QWidget* _r = openView((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = _r; }  break;
        case 15: setTimeLabel((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: setChunkLabel(); break;
        case 17: setNoteLabel(); break;
        case 18: setTimeRange((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 19: rewindPressed(); break;
        case 20: rewindReleased(); break;
        case 21: playStopClicked(); break;
        case 22: fastforwardPressed(); break;
        case 23: fastforwardReleased(); break;
        case 24: setTitle((*reinterpret_cast< Channel*(*)>(_a[1]))); break;
        case 25: aboutTartini(); break;
        case 26: aboutGPL(); break;
        case 27: aboutQt(); break;
        case 28: showDocumentation(); break;
        case 29: printPitch(); break;
        case 30: exportChannel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 31: exportChannelPlainText(); break;
        case 32: exportChannelMatlab(); break;
        case 33: { bool _r = loadViewGeometry();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 34: saveViewGeometry(); break;
        }
        _id -= 35;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::changedMusicFolder(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::zoomInPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void MainWindow::zoomOutPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
static const uint qt_meta_data_TartiniDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_TartiniDialog[] = {
    "TartiniDialog\0"
};

const QMetaObject TartiniDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TartiniDialog,
      qt_meta_data_TartiniDialog, 0 }
};

const QMetaObject *TartiniDialog::metaObject() const
{
    return &staticMetaObject;
}

void *TartiniDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TartiniDialog))
	return static_cast<void*>(const_cast< TartiniDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int TartiniDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_GPLDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_GPLDialog[] = {
    "GPLDialog\0"
};

const QMetaObject GPLDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GPLDialog,
      qt_meta_data_GPLDialog, 0 }
};

const QMetaObject *GPLDialog::metaObject() const
{
    return &staticMetaObject;
}

void *GPLDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GPLDialog))
	return static_cast<void*>(const_cast< GPLDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int GPLDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
