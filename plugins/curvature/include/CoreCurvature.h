#ifndef FACE_APP_SHARED_PLUGIN_CORE_CURVATURE_VIS_H
#define FACE_APP_SHARED_PLUGIN_CORE_CURVATURE_VIS_H

#include <FaceActionInterface.h>   // FaceTools
#include "plugins_3dfast_curvature_Export.h"

namespace FaceApp {
namespace Visualisation {

class plugins_3dfast_curvature_EXPORT CoreCurvature : public FaceTools::FaceActionGroup
{
Q_OBJECT
Q_PLUGIN_METADATA( IID FaceToolsPluginFaceActionInterface_iid)
Q_INTERFACES( FaceTools::FaceActionInterface)
public:
    CoreCurvature();
    virtual ~CoreCurvature(){}
};  // end class

}   // end namespace
}   // end namespace

#endif
