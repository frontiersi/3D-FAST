#include <PointsVisualisation.h>
#include <FaceControl.h>
#include <FaceView.h>
#include <vtkProperty.h>
#include <vtkMapper.h>
using FaceApp::PointsVisualisation;

PointsVisualisation::PointsVisualisation( const std::string& iconfile)
    : _icon( iconfile.c_str()), _keys( Qt::Key_1)
{
    init();
}   // end ctor


// public
void PointsVisualisation::mapActor( FaceTools::FaceControl* fcont)
{
    vtkActor* actor = fcont->getView()->getSurfaceActor();
    actor->GetProperty()->SetRepresentationToPoints();
    actor->GetMapper()->SetScalarVisibility(false);
}   // end mapActor


