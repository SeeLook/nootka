/****************************************************************************
** Meta object code from reading C++ file 'scorewidget.h'
**
** Created: Thu Jun 21 21:45:58 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/score/scorewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scorewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ScoreWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   13,   12,   12, 0x0a,
      35,   33,   12,   12, 0x0a,
      58,   53,   12,   12, 0x0a,
      77,   53,   12,   12, 0x0a,
      95,   53,   12,   12, 0x0a,
     112,   53,   12,   12, 0x0a,
     137,  131,   12,   12, 0x0a,
     160,   53,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ScoreWidget[] = {
    "ScoreWidget\0\0x\0setScaleX(double)\0y\0"
    "setScaleY(double)\0mode\0setSharpsMode(int)\0"
    "setNotesMode(int)\0setClefMode(int)\0"
    "setOpaqueMode(int)\0index\0"
    "setTransposeLevel(int)\0setShowAllMode(int)\0"
};

const QMetaObject ScoreWidget::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_ScoreWidget,
      qt_meta_data_ScoreWidget, 0 }
};

const QMetaObject *ScoreWidget::metaObject() const
{
    return &staticMetaObject;
}

void *ScoreWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScoreWidget))
	return static_cast<void*>(const_cast< ScoreWidget*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int ScoreWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setScaleX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: setScaleY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setSharpsMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setNotesMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: setClefMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: setOpaqueMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: setTransposeLevel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: setShowAllMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 8;
    }
    return _id;
}
