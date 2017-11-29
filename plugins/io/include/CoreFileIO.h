#ifndef FACE_APP_STATIC_PLUGIN_CORE_FILE_IO_H
#define FACE_APP_STATIC_PLUGIN_CORE_FILE_IO_H

#define QT_STATICPLUGIN
#include <FaceActionInterface.h>   // FaceTools

namespace FaceApp {

class CoreFileIO : public FaceTools::FaceActionGroup
{
Q_OBJECT
Q_PLUGIN_METADATA( IID FaceToolsPluginFaceActionInterface_iid)
Q_INTERFACES( FaceTools::FaceActionInterface)
public:
    CoreFileIO();
    virtual ~CoreFileIO(){}
};  // end class

}   // end namespace

#endif
