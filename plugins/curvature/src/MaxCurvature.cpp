#include <MaxCurvature.h>
#include <PolySurfaceCurvScalarMapper.h>
#include <FaceControl.h>
#include <VtkTools.h>
using FaceApp::Visualisation::MaxCurvature;
using RFeatures::ObjModelCurvatureMetrics;
using RFeatures::ObjModel;
using FaceTools::FaceControl;
typedef boost::unordered_map<int,int> IntIntMap;

MaxCurvature::MaxCurvature() : _icon(":/icons/grad_kp1.png"), _minv(0), _maxv(0) { init();}


namespace {
class MaxCurvMapper : public RVTK::PolySurfaceCurvScalarMapper
{
public:
    MaxCurvMapper( const ObjModelCurvatureMetrics::Ptr cm, vtkActor* actor, const IntIntMap* lookup, const std::string& metricName)
        : RVTK::PolySurfaceCurvScalarMapper(cm, actor, lookup, metricName)
    {}  // end ctor

protected:
    virtual float getCurvMetric( int faceIdx) const
    {
        assert(_cmetrics);
        return (float)_cmetrics->getFaceKP1FirstOrder(faceIdx);
    }   // end getCurvMetric
};  // end class
}   // end namespace


// public
bool MaxCurvature::isAvailable( const FaceControl* fcont) const
{
    return fcont->getModel()->getCurvatureMetrics() != NULL;
}   // end isAvailable


// public
void MaxCurvature::mapActor( FaceControl* fcont)
{
    const std::string vname = getDisplayName().toStdString();
    vtkActor* actor = fcont->getView()->getSurfaceActor();
    MaxCurvMapper mapper( fcont->getModel()->getCurvatureMetrics(), actor,
                          fcont->getView()->getPolyIdLookups(), vname);
    mapper.mapActor();
    mapper.getMappedRange( &_minv, &_maxv);

    vtkDataSetAttributes* da = RVTK::getPolyData( actor)->GetCellData();
    da->SetActiveScalars( vname.c_str());
    actor->GetMapper()->SetScalarVisibility(true);
}   // end mapActor


// public
bool MaxCurvature::allowScalarVisualisation( float& minv, float& maxv) const
{
    minv = _minv;
    maxv = _maxv;
    return true;
}   // end allowScalarVisualisation
