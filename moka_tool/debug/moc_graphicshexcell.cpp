/****************************************************************************
** Meta object code from reading C++ file 'graphicshexcell.h'
**
** Created: Fri Mar 22 10:18:59 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/gui/graphicshexcell.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'graphicshexcell.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gui__GraphicsHexCell[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   22,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_gui__GraphicsHexCell[] = {
    "gui::GraphicsHexCell\0\0row,col\0"
    "showUnitInfo(uint,uint)\0"
};

void gui::GraphicsHexCell::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GraphicsHexCell *_t = static_cast<GraphicsHexCell *>(_o);
        switch (_id) {
        case 0: _t->showUnitInfo((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData gui::GraphicsHexCell::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gui::GraphicsHexCell::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_gui__GraphicsHexCell,
      qt_meta_data_gui__GraphicsHexCell, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gui::GraphicsHexCell::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gui::GraphicsHexCell::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gui::GraphicsHexCell::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gui__GraphicsHexCell))
        return static_cast<void*>(const_cast< GraphicsHexCell*>(this));
    if (!strcmp(_clname, "QGraphicsPolygonItem"))
        return static_cast< QGraphicsPolygonItem*>(const_cast< GraphicsHexCell*>(this));
    return QObject::qt_metacast(_clname);
}

int gui::GraphicsHexCell::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void gui::GraphicsHexCell::showUnitInfo(uint _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
