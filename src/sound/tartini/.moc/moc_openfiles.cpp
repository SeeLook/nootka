/****************************************************************************
** Meta object code from reading C++ file 'openfiles.h'
**
** Created: Thu Jun 21 21:45:39 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/openfiles/openfiles.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'openfiles.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_OpenFiles[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      37,   32,   10,   10, 0x0a,
      70,   32,   10,   10, 0x0a,
     113,  106,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_OpenFiles[] = {
    "OpenFiles\0\0refreshChannelList()\0item\0"
    "listViewChanged(Q3ListViewItem*)\0"
    "slotCurrentChanged(Q3ListViewItem*)\0"
    "active\0slotActiveChannelChanged(Channel*)\0"
};

const QMetaObject OpenFiles::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_OpenFiles,
      qt_meta_data_OpenFiles, 0 }
};

const QMetaObject *OpenFiles::metaObject() const
{
    return &staticMetaObject;
}

void *OpenFiles::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OpenFiles))
	return static_cast<void*>(const_cast< OpenFiles*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int OpenFiles::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refreshChannelList(); break;
        case 1: listViewChanged((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 2: slotCurrentChanged((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 3: slotActiveChannelChanged((*reinterpret_cast< Channel*(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}
