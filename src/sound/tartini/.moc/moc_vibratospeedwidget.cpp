/****************************************************************************
** Meta object code from reading C++ file 'vibratospeedwidget.h'
**
** Created: Thu Jun 21 21:46:01 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/vibrato/vibratospeedwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vibratospeedwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_VibratoSpeedWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x0a,
      37,   31,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VibratoSpeedWidget[] = {
    "VibratoSpeedWidget\0\0doUpdate()\0value\0"
    "setUseProny(bool)\0"
};

const QMetaObject VibratoSpeedWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_VibratoSpeedWidget,
      qt_meta_data_VibratoSpeedWidget, 0 }
};

const QMetaObject *VibratoSpeedWidget::metaObject() const
{
    return &staticMetaObject;
}

void *VibratoSpeedWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VibratoSpeedWidget))
	return static_cast<void*>(const_cast< VibratoSpeedWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int VibratoSpeedWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doUpdate(); break;
        case 1: setUseProny((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
