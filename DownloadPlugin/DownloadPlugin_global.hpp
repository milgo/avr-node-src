#ifndef DOWNLOADPLUGIN_GLOBAL_H
#define DOWNLOADPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DOWNLOADPLUGIN_LIBRARY)
#  define DOWNLOADPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DOWNLOADPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DOWNLOADPLUGIN_GLOBAL_H
