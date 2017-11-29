#include <UniformDistance.h>
#include <PolySurfaceDistanceScalarMapper.h>    // RVTK
#include <FaceControl.h>
#include <FaceModel.h>
#include <FaceView.h>
#include <VtkTools.h>
#include <cassert>
using FaceApp::Visualisation::UniformDistance;
using FaceTools::FaceControl;
typedef boost::unordered_map<int,int> IntIntMap;


UniformDistance::UniformDistance() : _icon( ":DISTANCE_ICON"), _minv(0), _maxv(0) { init();}


// public
bool UniformDistance::isAvailable( const FaceControl* fcont) const
{
    return fcont->getModel()->getUniformDistanceMap() != NULL;
}   // end isAvailable


// public
void UniformDistance::mapActor( FaceControl* fcont)
{
    assert(isAvailable(fcont));
    vtkActor* actor = fcont->getView()->getSurfaceActor();
    const std::string vname = getDisplayName().toStdString();
    RVTK::PolySurfaceDistanceScalarMapper mapper( fcont->getModel()->getObjectMeta()->getObject(),
                                                  actor,  
                                                  fcont->getView()->getPolyIdLookups(),
                                                  *fcont->getModel()->getUniformDistanceMap(),
                                                  vname);
    mapper.mapActor();
    mapper.getMappedRange( &_minv, &_maxv);

    vtkDataSetAttributes* da = RVTK::getPolyData( actor)->GetCellData();
    da->SetActiveScalars( vname.c_str());
    actor->GetMapper()->SetScalarVisibility(true);
}   // end mapActor


// public
bool UniformDistance::allowScalarVisualisation( float& minv, float& maxv) const
{
    minv = _minv;
    maxv = _maxv;
    return true;
}   // end allowScalarVisualisation

