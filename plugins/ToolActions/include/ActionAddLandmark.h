#ifndef FACE_APP_ACTION_ADD_LANDMARK_H
#define FACE_APP_ACTION_ADD_LANDMARK_H

#include <FaceActionInterface.h>

namespace FaceApp {

class ActionAddLandmark : public FaceTools::FaceAction
{ Q_OBJECT
public:
    ActionAddLandmark( const std::string& dname, const std::string& iconfilename="");

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return _dname;}

    void setControlled( FaceTools::FaceControl*, bool) override;

protected:
    bool doAction() override;

private slots:
    void checkEnable();

private:
    const QIcon _icon;
    const QString _dname;
    FaceTools::FaceControl* _fcont;
};  // end class

}   // end namespace

#endif

