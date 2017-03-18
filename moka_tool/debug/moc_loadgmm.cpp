/****************************************************************************
** Meta object code from reading C++ file 'loadgmm.h'
**
** Created: Fri Mar 22 10:18:46 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/thread/loadgmm.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'loadgmm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_thread__LoadGMM[] = {

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
      26,   17,   16,   16, 0x05,
      50,   43,   16,   16, 0x05,
      69,   17,   16,   16, 0x05,
      88,   16,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_thread__LoadGMM[] = {
    "thread::LoadGMM\0\0filename\0loadEnd(QString)\0"
    "reason\0loadError(QString)\0loadStart(QString)\0"
    "mutexIsLocked()\0"
};

void thread::LoadGMM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LoadGMM *_t = static_cast<LoadGMM *>(_o);
        switch (_id) {
        case 0: _t->loadEnd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->loadError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->loadStart((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->mutexIsLocked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData thread::LoadGMM::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject thread::LoadGMM::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_thread__LoadGMM,
      qt_meta_data_thread__LoadGMM, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &thread::LoadGMM::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *thread::LoadGMM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *thread::LoadGMM::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_thread__LoadGMM))
        return static_cast<void*>(const_cast< LoadGMM*>(this));
    return QThread::qt_metacast(_clname);
}

int thread::LoadGMM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void thread::LoadGMM::loadEnd(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void thread::LoadGMM::loadError(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void thread::LoadGMM::loadStart(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void thread::LoadGMM::mutexIsLocked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
