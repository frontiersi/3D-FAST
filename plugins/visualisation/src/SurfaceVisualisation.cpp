#include <SurfaceVisualisation.h>
#include <FaceControl.h>
#include <FaceView.h>
#include <vtkProperty.h>
#include <vtkMapper.h>
using FaceApp::SurfaceVisualisation;

SurfaceVisualisation::SurfaceVisualisation( const std::string& iconfile)
    : _icon( iconfile.c_str()), _keys( Qt::Key_3)
{
    init();
}   // end ctor


// public
void SurfaceVisualisation::mapActor( FaceTools::FaceControl* fcont)
{
    vtkActor* actor = fcont->getView()->getSurfaceActor();
    actor->GetProperty()->SetRepresentationToSurface();
    actor->GetMapper()->SetScalarVisibility(false);
}   // end mapActor


// protected
bool SurfaceVisualisation::isDefault( const FaceTools::FaceControl* fcont) const
{
    return fcont->getModel()->getObjectMeta()->getObject()->getNumMaterials() == 0;
}   // end isDefault
