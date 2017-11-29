#ifndef FACE_APP_ACTION_DETECT_FACE_H
#define FACE_APP_ACTION_DETECT_FACE_H

#include <ActionProcessModel.h>
#include <FaceDetector.h>

namespace FaceApp {

class ActionDetectFace : public FaceTools::ActionProcessModel
{ Q_OBJECT
public:
    ActionDetectFace( const std::string& dname, const std::string& iconfilename="");

    void initFaceDetection( const QString& haarModelsDir, const QString& faceLmksShapeModel);

    bool operator()( FaceTools::FaceControl*) override;
    bool isActionable( FaceTools::FaceControl*) const override;

private:
    FaceTools::FaceDetector::Ptr _faceDetector;
};  // end class

}   // end namespace

#endif

