#include <NormalDeterminant.h>
#include <PolySurfaceCurvScalarMapper.h>
#include <FaceControl.h>
#include <VtkTools.h>
using FaceApp::Visualisation::NormalDeterminant;
using RFeatures::ObjModelCurvatureMetrics;
using RFeatures::ObjModel;
using FaceTools::FaceControl;
typedef boost::unordered_map<int,int> IntIntMap;

NormalDeterminant::NormalDeterminant() : _icon(":/icons/grad_determinant.png"), _minv(0), _maxv(0) { init();}

namespace {
class NormalDeterminantMapper : public RVTK::PolySurfaceCurvScalarMapper
{
public:
    NormalDeterminantMapper( const ObjModelCurvatureMetrics::Ptr cm, vtkActor* actor, const IntIntMap* lookup, const std::string& metricName)
        : RVTK::PolySurfaceCurvScalarMapper(cm, actor, lookup, metricName)
    {}  // end ctor

protected:
    virtual float getCurvMetric( int faceIdx) const
    {
        return _cmetrics->getFaceDeterminant(faceIdx);
    }   // end getCurvMetric
};  // end class
}   // end namespace


// public
bool NormalDeterminant::isAvailable( const FaceControl* fcont) const
{
    return fcont->getModel()->getCurvatureMetrics() != NULL;
}   // end isAvailable


// public
void NormalDeterminant::mapActor( FaceControl* fcont)
{
    const std::string vname = getDisplayName().toStdString();
    vtkActor* actor = fcont->getView()->getSurfaceActor();
    NormalDeterminantMapper mapper( fcont->getModel()->getCurvatureMetrics(), actor,
                                    fcont->getView()->getPolyIdLookups(), vname);
    mapper.mapActor();
    mapper.getMappedRange( &_minv, &_maxv);

    vtkDataSetAttributes* da = RVTK::getPolyData( actor)->GetCellData();
    da->SetActiveScalars( vname.c_str());
    actor->GetMapper()->SetScalarVisibility(true);
}   // end mapActor


// public
bool NormalDeterminant::allowScalarVisualisation( float& minv, float& maxv) const
{
    minv = _minv;
    maxv = _maxv;
    return true;
}   // end allowScalarVisualisation
