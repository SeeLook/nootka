/****************************************************************************
** Meta object code from reading C++ file 'hbubblewidget.h'
**
** Created: Thu Jun 21 21:45:47 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/hbubble/hbubblewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hbubblewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HBubbleWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      43,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      72,   14,   14,   14, 0x0a,
      96,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_HBubbleWidget[] = {
    "HBubbleWidget\0\0numHarmonicsChanged(double)\0"
    "historyChunksChanged(double)\0"
    "setNumHarmonics(double)\0"
    "setHistoryChunks(double)\0"
};

const QMetaObject HBubbleWidget::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_HBubbleWidget,
      qt_meta_data_HBubbleWidget, 0 }
};

const QMetaObject *HBubbleWidget::metaObject() const
{
    return &staticMetaObject;
}

void *HBubbleWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HBubbleWidget))
	return static_cast<void*>(const_cast< HBubbleWidget*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int HBubbleWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: numHarmonicsChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: historyChunksChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setNumHarmonics((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: setHistoryChunks((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void HBubbleWidget::numHarmonicsChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HBubbleWidget::historyChunksChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
