#include <WireframeVisualisation.h>
#include <FaceView.h>
#include <FaceControl.h>
#include <vtkProperty.h>
#include <vtkMapper.h>
using FaceApp::WireframeVisualisation;

WireframeVisualisation::WireframeVisualisation( const std::string& iconfile)
    : _icon( iconfile.c_str())
{
    init();
}   // end ctor


// public
void WireframeVisualisation::mapActor( FaceTools::FaceControl* fcont)
{
    vtkActor* actor = fcont->getView()->getSurfaceActor();
    actor->GetProperty()->SetRepresentationToWireframe();
    actor->GetMapper()->SetScalarVisibility(false);
}   // end mapActor


