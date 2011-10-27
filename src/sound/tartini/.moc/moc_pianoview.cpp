/****************************************************************************
** Meta object code from reading C++ file 'pianoview.h'
**
** Created: Thu Jun 21 21:45:53 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/piano/pianoview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pianoview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_PianoView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PianoView[] = {
    "PianoView\0\0changeKey()\0"
};

const QMetaObject PianoView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_PianoView,
      qt_meta_data_PianoView, 0 }
};

const QMetaObject *PianoView::metaObject() const
{
    return &staticMetaObject;
}

void *PianoView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PianoView))
	return static_cast<void*>(const_cast< PianoView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int PianoView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changeKey(); break;
        }
        _id -= 1;
    }
    return _id;
}
