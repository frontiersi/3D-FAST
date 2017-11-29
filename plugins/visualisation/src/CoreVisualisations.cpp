#include <CoreVisualisations.h>
#include <TextureVisualisation.h>
#include <SurfaceVisualisation.h>
#include <WireframeVisualisation.h>
#include <PointsVisualisation.h>
using FaceApp::CoreVisualisations;

void initCoreVis()
{
    Q_INIT_RESOURCE(iconsv);
}   // end initCoreVis

CoreVisualisations::CoreVisualisations() : FaceTools::FaceActionGroup()
{
    addAction( new FaceApp::TextureVisualisation(":TEXTURE_ICON"));
    addAction( new FaceApp::SurfaceVisualisation(":SURFACE_ICON"));
    addAction( new FaceApp::WireframeVisualisation(":WIREFRAME_ICON"));
    addAction( new FaceApp::PointsVisualisation(":POINTS_ICON"));
}   // end ctor
