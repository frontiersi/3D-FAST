#include <MeanCurvature.h>
#include <PolySurfaceCurvScalarMapper.h>
#include <FaceControl.h>
#include <VtkTools.h>
using FaceApp::Visualisation::MeanCurvature;
using RFeatures::ObjModelCurvatureMetrics;
using RFeatures::ObjModel;
using FaceTools::FaceControl;
typedef boost::unordered_map<int,int> IntIntMap;

MeanCurvature::MeanCurvature() : _icon(":/icons/grad_mean.png"), _minv(0), _maxv(0) { init();}


namespace {
class MeanCurvMapper : public RVTK::PolySurfaceCurvScalarMapper
{
public:
    MeanCurvMapper( const ObjModelCurvatureMetrics::Ptr cm, vtkActor* actor, const IntIntMap* lookup, const std::string& metricName)
        : RVTK::PolySurfaceCurvScalarMapper(cm, actor, lookup, metricName)
    {}  // end ctor

protected:
    virtual float getCurvMetric( int faceIdx) const
    {
        return (_cmetrics->getFaceKP1FirstOrder( faceIdx) + _cmetrics->getFaceKP2FirstOrder( faceIdx)) / 2;
    }   // end getCurvMetric
};  // end class
}   // end namespace


// public
bool MeanCurvature::isAvailable( const FaceControl* fcont) const
{
    return fcont->getModel()->getCurvatureMetrics() != NULL;
}   // end isAvailable


// public
void MeanCurvature::mapActor( FaceControl* fcont)
{
    const std::string vname = getDisplayName().toStdString();
    vtkActor *actor = fcont->getView()->getSurfaceActor();
    MeanCurvMapper mapper( fcont->getModel()->getCurvatureMetrics(), actor,
                           fcont->getView()->getPolyIdLookups(), vname);
    mapper.mapActor();
    mapper.getMappedRange( &_minv, &_maxv);

    vtkDataSetAttributes* da = RVTK::getPolyData( actor)->GetCellData();
    da->SetActiveScalars( vname.c_str());
    actor->GetMapper()->SetScalarVisibility(true);
}   // end mapActor


// public
bool MeanCurvature::allowScalarVisualisation( float& minv, float& maxv) const
{
    minv = _minv;
    maxv = _maxv;
    return true;
}   // end allowScalarVisualisation
