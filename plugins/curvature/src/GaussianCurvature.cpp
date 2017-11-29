#include <GaussianCurvature.h>
#include <PolySurfaceCurvScalarMapper.h>
#include <FaceControl.h>
#include <VtkTools.h>
using FaceApp::Visualisation::GaussianCurvature;
using RFeatures::ObjModelCurvatureMetrics;
using RFeatures::ObjModel;
using FaceTools::FaceControl;
typedef boost::unordered_map<int,int> IntIntMap;

GaussianCurvature::GaussianCurvature() : _icon(":/icons/grad_gaussian.png"), _minv(0), _maxv(0) { init();}

namespace {
class GaussianCurvMapper : public RVTK::PolySurfaceCurvScalarMapper
{
public:
    GaussianCurvMapper( const ObjModelCurvatureMetrics::Ptr cm, vtkActor* actor, const IntIntMap* lookup, const std::string& metricName)
        : RVTK::PolySurfaceCurvScalarMapper(cm, actor, lookup, metricName)
    {}  // end ctor

protected:
    virtual float getCurvMetric( int faceIdx) const
    {
        return _cmetrics->getFaceKP1FirstOrder( faceIdx) * _cmetrics->getFaceKP2FirstOrder( faceIdx);
    }   // end getCurvMetric
};  // end class
}   // end namespace


// public
bool GaussianCurvature::isAvailable( const FaceControl* fcont) const
{
    return fcont->getModel()->getCurvatureMetrics() != NULL;
}   // end isAvailable


// public
void GaussianCurvature::mapActor( FaceControl* fcont)
{
    const std::string vname = getDisplayName().toStdString();
    vtkActor *actor = fcont->getView()->getSurfaceActor();
    GaussianCurvMapper mapper( fcont->getModel()->getCurvatureMetrics(), actor,
                               fcont->getView()->getPolyIdLookups(), vname);
    mapper.mapActor();
    mapper.getMappedRange( &_minv, &_maxv);

    vtkDataSetAttributes* da = RVTK::getPolyData( actor)->GetCellData();
    da->SetActiveScalars( vname.c_str());
    actor->GetMapper()->SetScalarVisibility(true);
}   // end mapActor


// public
bool GaussianCurvature::allowScalarVisualisation( float& minv, float& maxv) const
{
    minv = _minv;
    maxv = _maxv;
    return true;
}   // end allowScalarVisualisation
