/****************************************************************************
** Meta object code from reading C++ file 'vibratotunerwidget.h'
**
** Created: Thu Jun 21 21:46:00 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/vibrato/vibratotunerwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vibratotunerwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_VibratoTunerWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      32,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   49,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VibratoTunerWidget[] = {
    "VibratoTunerWidget\0\0index,value\0"
    "ledSet(int,bool)\0thePitch\0doUpdate(double)\0"
};

const QMetaObject VibratoTunerWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_VibratoTunerWidget,
      qt_meta_data_VibratoTunerWidget, 0 }
};

const QMetaObject *VibratoTunerWidget::metaObject() const
{
    return &staticMetaObject;
}

void *VibratoTunerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VibratoTunerWidget))
	return static_cast<void*>(const_cast< VibratoTunerWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int VibratoTunerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: ledSet((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: doUpdate((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void VibratoTunerWidget::ledSet(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
