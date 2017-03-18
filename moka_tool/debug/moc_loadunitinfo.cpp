/****************************************************************************
** Meta object code from reading C++ file 'loadunitinfo.h'
**
** Created: Fri Mar 22 10:18:59 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/thread/loadunitinfo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'loadunitinfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_thread__LoadUnitInfo[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   22,   21,   21, 0x05,
      55,   48,   21,   21, 0x05,
      74,   22,   21,   21, 0x05,
      93,   21,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_thread__LoadUnitInfo[] = {
    "thread::LoadUnitInfo\0\0filename\0"
    "loadEnd(QString)\0reason\0loadError(QString)\0"
    "loadStart(QString)\0mutexIsLocked()\0"
};

void thread::LoadUnitInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LoadUnitInfo *_t = static_cast<LoadUnitInfo *>(_o);
        switch (_id) {
        case 0: _t->loadEnd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->loadError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->loadStart((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->mutexIsLocked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData thread::LoadUnitInfo::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject thread::LoadUnitInfo::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_thread__LoadUnitInfo,
      qt_meta_data_thread__LoadUnitInfo, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &thread::LoadUnitInfo::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *thread::LoadUnitInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *thread::LoadUnitInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_thread__LoadUnitInfo))
        return static_cast<void*>(const_cast< LoadUnitInfo*>(this));
    return QThread::qt_metacast(_clname);
}

int thread::LoadUnitInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void thread::LoadUnitInfo::loadEnd(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void thread::LoadUnitInfo::loadError(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void thread::LoadUnitInfo::loadStart(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void thread::LoadUnitInfo::mutexIsLocked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
