#ifndef FACE_APP_SHARED_PLUGIN_CORE_DISTANCES_VIS_H
#define FACE_APP_SHARED_PLUGIN_CORE_DISTANCES_VIS_H

#include <FaceActionInterface.h>   // FaceTools
#include "plugins_3dfast_distances_Export.h"

namespace FaceApp {
namespace Visualisation {

class plugins_3dfast_distances_EXPORT CoreDistances : public FaceTools::FaceActionGroup
{
Q_OBJECT
Q_PLUGIN_METADATA( IID FaceToolsPluginFaceActionInterface_iid)
Q_INTERFACES( FaceTools::FaceActionInterface)
public:
    CoreDistances();
    virtual ~CoreDistances(){}
};  // end class

}   // end namespace
}   // end namespace

#endif

