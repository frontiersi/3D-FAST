#ifndef FACE_APP_SHARED_PLUGIN_GAUSSIAN_CURVATURE_VIS_H
#define FACE_APP_SHARED_PLUGIN_GAUSSIAN_CURVATURE_VIS_H

#include <VisualisationAction.h>   // FaceTools
#include "plugins_3dfast_curvature_Export.h"

namespace FaceApp {
namespace Visualisation {

class plugins_3dfast_curvature_EXPORT GaussianCurvature : public FaceTools::VisualisationAction
{ Q_OBJECT
public:
    GaussianCurvature();

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return "Gaus. Curvature";}

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



