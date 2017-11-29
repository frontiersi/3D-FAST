#ifndef FACE_APP_ACTION_MOVE_LANDMARKS_H
#define FACE_APP_ACTION_MOVE_LANDMARKS_H

#include <FaceActionInterface.h>
#include <FaceControl.h>
#include <Landmarks.h>

namespace FaceApp {

class ActionMoveLandmarks : public FaceTools::FaceAction
{ Q_OBJECT
public:
    ActionMoveLandmarks( const std::string& dname, const std::string& iconfilename);

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return _dname;}

    void setControlled( FaceTools::FaceControl*, bool) override;

protected:
    bool doAction() override;

private slots:
    void checkEnable();
    void selectLandmark( const QPoint&);
    void deselectLandmark( const QPoint&);
    void moveLandmark( const QPoint&);

private:
    const QIcon _icon;
    const QString _dname;
    boost::unordered_set<FaceTools::FaceControl*> _fconts;
    FaceTools::Landmarks::Landmark* _lmk;
};  // end class

}   // end namespace

#endif


