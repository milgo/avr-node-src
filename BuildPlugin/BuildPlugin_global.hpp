#ifndef BUILDPLUGIN_GLOBAL_H
#define BUILDPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BUILDPLUGIN_LIBRARY)
#  define BUILDPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BUILDPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BUILDPLUGIN_GLOBAL_H
