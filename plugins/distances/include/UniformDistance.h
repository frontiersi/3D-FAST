#ifndef FACE_APP_SHARED_PLUGIN_UNIFORM_DISTANCE_VIS_H
#define FACE_APP_SHARED_PLUGIN_UNIFORM_DISTANCE_VIS_H

#include <VisualisationAction.h>   // FaceTools
#include "plugins_3dfast_distances_Export.h"

namespace FaceApp {
namespace Visualisation {

class plugins_3dfast_distances_EXPORT UniformDistance : public FaceTools::VisualisationAction
{ Q_OBJECT
public:
    UniformDistance();

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return "Uniform Distance";}

    bool isAvailable( const FaceTools::FaceControl*) const override;
    bool allowScalarVisualisation( float& minv, float& maxv) const override;

    void mapActor( FaceTools::FaceControl*) override;

private:
    const QIcon _icon;
    float _minv, _maxv;
};  // end class

}   // end namespace
}   // end namespace

#endif



