/****************************************************************************
** Meta object code from reading C++ file 'wavewidget.h'
**
** Created: Thu Jun 21 21:45:49 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/wave/wavewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wavewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WaveWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   12,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WaveWidget[] = {
    "WaveWidget\0\0zoomY_\0zoomYChanged(double)\0"
    "setZoomY(double)\0"
};

const QMetaObject WaveWidget::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_WaveWidget,
      qt_meta_data_WaveWidget, 0 }
};

const QMetaObject *WaveWidget::metaObject() const
{
    return &staticMetaObject;
}

void *WaveWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WaveWidget))
	return static_cast<void*>(const_cast< WaveWidget*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int WaveWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: zoomYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: setZoomY((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void WaveWidget::zoomYChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
