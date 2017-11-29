#include <CoreDistances.h>
#include <UniformDistance.h>
using FaceApp::Visualisation::CoreDistances;

void initResources()
{
    Q_INIT_RESOURCE(icons);
}   // end initResources

CoreDistances::CoreDistances() : FaceTools::FaceActionGroup()
{
    initResources();
    addAction( new FaceApp::Visualisation::UniformDistance);
}   // end ctor


