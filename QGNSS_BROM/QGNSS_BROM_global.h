#ifndef QGNSS_BROM_GLOBAL_H
#define QGNSS_BROM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QGNSS_BROM_LIBRARY)
#  define QGNSS_BROM_EXPORT Q_DECL_EXPORT
#else
#  define QGNSS_BROM_EXPORT Q_DECL_IMPORT
#endif

#endif // QGNSS_BROM_GLOBAL_H
