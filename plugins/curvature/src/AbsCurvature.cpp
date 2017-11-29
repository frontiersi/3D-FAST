#include <AbsCurvature.h>
#include <PolySurfaceCurvScalarMapper.h>
#include <FaceControl.h>
#include <VtkTools.h>
using FaceApp::Visualisation::AbsCurvature;
using RFeatures::ObjModelCurvatureMetrics;
using RFeatures::ObjModel;
using FaceTools::FaceControl;
typedef boost::unordered_map<int,int> IntIntMap;

AbsCurvature::AbsCurvature() : _icon(":/icons/grad_abs.png"), _minv(0), _maxv(0) { init();}

namespace {
class AbsCurvMapper : public RVTK::PolySurfaceCurvScalarMapper
{
public:
    AbsCurvMapper( const ObjModelCurvatureMetrics::Ptr cm, vtkActor* actor, const IntIntMap* lookup, const std::string& metricName)
        : RVTK::PolySurfaceCurvScalarMapper(cm, actor, lookup, metricName)
    {}  // end ctor

protected:
    virtual float getCurvMetric( int faceIdx) const
    {
        return sqrt( (pow(_cmetrics->getFaceKP1FirstOrder( faceIdx),2) + pow(_cmetrics->getFaceKP2FirstOrder( faceIdx),2))/2);
    }   // end getCurvMetric
};  // end class
}   // end namespace


// public
bool AbsCurvature::isAvailable( const FaceControl* fcont) const
{
    return fcont->getModel()->getCurvatureMetrics() != NULL;
}   // end isAvailable


// public
void AbsCurvature::mapActor( FaceControl* fcont)
{
    const std::string vname = getDisplayName().toStdString();
    vtkActor* actor = fcont->getView()->getSurfaceActor();
    AbsCurvMapper mapper( fcont->getModel()->getCurvatureMetrics(), actor,
                          fcont->getView()->getPolyIdLookups(), vname);
    mapper.mapActor();
    mapper.getMappedRange( &_minv, &_maxv);

    vtkDataSetAttributes* da = RVTK::getPolyData( actor)->GetCellData();
    da->SetActiveScalars( vname.c_str());
    actor->GetMapper()->SetScalarVisibility(true);
}   // end mapActor


// public
bool AbsCurvature::allowScalarVisualisation( float& minv, float& maxv) const
{
    minv = _minv;
    maxv = _maxv;
    return true;
}   // end allowScalarVisualisation
