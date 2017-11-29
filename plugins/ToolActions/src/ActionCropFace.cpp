#include <ActionCropFace.h>
#include <FaceTools.h>
#include <Landmarks.h>      // FaceTools
#include <ObjMetaData.h>    // FaceTools
#include <ObjModelVertexAdder.h>    // RFeatures
#include <ObjModelSmoother.h>       // RFeatures
#include <ObjModelHoleFiller.h>     // RFeatures
#include <cassert>
using FaceApp::ActionCropFace;
using FaceTools::FaceControl;
using FaceTools::FaceModel;


ActionCropFace::ActionCropFace( const std::string& dname, const std::string& fname)
    : FaceTools::ActionProcessModel( dname, fname, true, true, true)
{
}   // end ctor


bool ActionCropFace::operator()( FaceControl* fcont)
{
    FaceModel* fmodel = fcont->getModel();

    FaceTools::ObjMetaData::Ptr omd = fmodel->getObjectMeta();
    assert( omd->getKDTree());
    double crad = FaceTools::calcFaceCropRadius( omd, fcont->getOptions().boundary.cropFactor + 0.2);
    if ( crad <= 0)
        return false;

    const cv::Vec3f fc = FaceTools::calcFaceCentre(omd);
    const std::string units = fcont->getOptions().munits;

    std::cerr << "[INFO] FaceTools::ActionCropFace::operator(): Crop #1 within " << crad << " " << units << " of face centre..." << std::endl;
    const int svid = omd->getKDTree()->find( omd->getLandmark( FaceTools::Landmarks::NASAL_TIP));
    RFeatures::ObjModel::Ptr cobj = FaceTools::crop( omd->getObject(), fc, crad, svid);

    std::cerr << "[INFO] FaceTools::ActionCropFace::operator(): Filling holes... ";
    const int nfilled = RFeatures::ObjModelHoleFiller::fillHoles( cobj) - 1;
    if ( nfilled > 0)
    {
        std::cerr << nfilled << " filled";
        FaceTools::clean( cobj);
    }   // end if
    else
        std::cerr << "none found";
    std::cerr << std::endl;

    // Increase vertex density
    const double mta = fcont->getOptions().maxTriangleArea;
    std::cerr << "[INFO] FaceTools::ActionCropFace::operator(): Subdividing and merging polys with area > "
              << mta << " " << units << "^2..." << std::endl;
    RFeatures::ObjModelVertexAdder vadder(cobj);
    vadder.subdivideAndMerge( mta);

    // Crop #2 (for smoother boundary)
    crad = FaceTools::calcFaceCropRadius( omd, fcont->getOptions().boundary.cropFactor);
    std::cerr << "[INFO] FaceTools::ActionCropFace::operator(): Crop #2 within " << crad << " " << units << " of face centre..." << std::endl;
    cobj = FaceTools::crop( cobj, fc, crad, 0);

    // Smooth
    const double sfactor = fcont->getOptions().smoothFactor;
    std::cerr << "[INFO] FaceTools::ActionCropFace::operator(): Smoothing (" << sfactor << " factor over max 10 iterations)..." << std::endl;
    RFeatures::ObjModelCurvatureMap::Ptr cmap = RFeatures::ObjModelCurvatureMap::create( cobj, *cobj->getFaceIds(0).begin());
    size_t numSmoothIterations = 10;
    RFeatures::ObjModelSmoother( cmap).smooth( sfactor, numSmoothIterations);

    const double lmshift = omd->shiftLandmarksToSurface();    // Map landmarks back to the surface (which changed from the smooth operation).
    std::cerr << "[INFO] FaceTools::ActionCropFace::operator(): Shifted landmarks to surface by mean of " << lmshift << " " << units << std::endl;
    // Shifting the landmarks slightly will mean the face centre is slightly different, but it will be a small difference that can be ignored.

    assert( cobj != NULL);
    fcont->updateMesh(cobj);
    return true;
}   // end operator()


bool ActionCropFace::isActionable( FaceControl* fcont) const
{
    return fcont->isBoundaryShown() && FaceTools::hasReqLandmarks( fcont->getModel()->getObjectMeta());
}   // end isActionable
