/****************************************************************************
** Meta object code from reading C++ file 'vibratowidget.h'
**
** Created: Thu Jun 21 21:45:59 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/vibrato/vibratowidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vibratowidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_VibratoWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      28,   26,   14,   14, 0x0a,
      53,   51,   14,   14, 0x0a,
      82,   76,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VibratoWidget[] = {
    "VibratoWidget\0\0doUpdate()\0x\0"
    "setZoomFactorX(double)\0y\0"
    "setZoomFactorY(double)\0value\0"
    "setOffsetY(int)\0"
};

const QMetaObject VibratoWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_VibratoWidget,
      qt_meta_data_VibratoWidget, 0 }
};

const QMetaObject *VibratoWidget::metaObject() const
{
    return &staticMetaObject;
}

void *VibratoWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VibratoWidget))
	return static_cast<void*>(const_cast< VibratoWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int VibratoWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doUpdate(); break;
        case 1: setZoomFactorX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setZoomFactorY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: setOffsetY((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}
