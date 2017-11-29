#ifndef FACE_APP_STATIC_PLUGIN_CORE_VISUALISATIONS_H
#define FACE_APP_STATIC_PLUGIN_CORE_VISUALISATIONS_H

#define QT_STATICPLUGIN
#include <FaceActionInterface.h> // FaceTools

namespace FaceApp {

class CoreVisualisations : public FaceTools::FaceActionGroup
{
Q_OBJECT
Q_PLUGIN_METADATA( IID FaceToolsPluginFaceActionInterface_iid)
Q_INTERFACES( FaceTools::FaceActionInterface)
public:
    CoreVisualisations();
    virtual ~CoreVisualisations(){}
};  // end class

}   // end namespace

#endif
