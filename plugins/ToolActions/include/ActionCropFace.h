#ifndef FACE_APP_ACTION_CROP_FACE_H
#define FACE_APP_ACTION_CROP_FACE_H

#include <ActionProcessModel.h>

namespace FaceApp {

class ActionCropFace : public FaceTools::ActionProcessModel
{ Q_OBJECT
public:
    ActionCropFace( const std::string& dname, const std::string& iconfilename="");

    bool operator()( FaceTools::FaceControl*) override;
    bool isActionable( FaceTools::FaceControl*) const override;
};  // end class

}   // end namespace

#endif

