#include <ActionDetectFace.h>
#include <ActionOrientCameraToFace.h>
#include <ObjModelHoleFiller.h>
#include <FaceTools.h>
#include <QMessageBox>
#include <cassert>
using FaceApp::ActionDetectFace;
using FaceTools::FaceControl;
using FaceTools::ObjMetaData;
using FaceTools::FaceModel;
using RFeatures::ObjModel;


ActionDetectFace::ActionDetectFace( const std::string& dname, const std::string& fname)
    : FaceTools::ActionProcessModel( dname, fname, false, false, false)
{
}   // end ctor


bool ActionDetectFace::operator()( FaceControl* fcont)
{
    const FaceModel* fmodel = fcont->getModel();
    ObjMetaData::Ptr omd = fmodel->getObjectMeta();
    if ( omd->hasLandmarks())
    {
        QMessageBox mb( tr("(Re)detect Face"), tr("Overwrite existing detection? This will reset landmarks!"),
                QMessageBox::Warning,
                QMessageBox::Yes,
                QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
                QMessageBox::NoButton);
        if ( mb.exec() == QMessageBox::No)
            return false;
    }   // end if

    progress( 0.1f);

    if ( !_faceDetector->detect( omd))  // Ensures KD tree built
    {
        const std::string err = _faceDetector->err();
        QMessageBox::warning( NULL, tr("Face Detection"), tr("Unable to detect face!"));
        return false;
    }   // end if
    std::cerr << "Detected face" << std::endl;
    progress( 0.7f);

    // Get the component attached to the face
    assert( omd->hasLandmark( FaceTools::Landmarks::NASAL_TIP));
    const cv::Vec3f& v = omd->getLandmark( FaceTools::Landmarks::NASAL_TIP);
    const int vidx = omd->getKDTree()->find( v);

    ObjModel::Ptr comp = FaceTools::getComponent( omd->getObject(), vidx);
    std::cerr << "Got face component" << std::endl;
    progress( 0.72f);

    // Hole fill
    const int nfilled = RFeatures::ObjModelHoleFiller::fillHoles( comp) - 1;
    progress( 0.82f);
    if ( nfilled > 0)
    {
        std::cerr << nfilled << " holes filled" << std::endl;
        FaceTools::clean( comp);
    }   // end if
    progress( 0.91f);

    omd->setObject( comp, false);   // Don't rebuild KD tree yet
    FaceTools::transformToOrigin( omd);
    progress( 0.93f);

    fcont->updateMesh(comp);
    progress( 0.99f);

    FaceTools::ActionOrientCameraToFace()( fcont);
    progress( 1.00f);
    fcont->showLandmarks(true);
    fcont->showBoundary(true);
    return true;
}   // end operator()


bool ActionDetectFace::isActionable( FaceControl* fcont) const
{
    return _faceDetector != NULL;
}   // end isActionable


// public
void ActionDetectFace::initFaceDetection( const QString& haarModelsDir, const QString& faceLmksShapeModel)
{
    _faceDetector = FaceTools::FaceDetector::create( haarModelsDir.toStdString(), faceLmksShapeModel.toStdString());
}   // end initFaceDetection
