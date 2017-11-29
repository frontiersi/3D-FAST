#ifndef FACE_APP_ACTION_TOGGLE_SHOW_LANDMARKS_H
#define FACE_APP_ACTION_TOGGLE_SHOW_LANDMARKS_H

#include <FaceActionInterface.h>
#include <FaceControl.h>

namespace FaceApp {

class ActionToggleShowLandmarks : public FaceTools::FaceAction
{ Q_OBJECT
public:
    ActionToggleShowLandmarks( const std::string& dname, const std::string& iconfilename);

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return _dname;}

    void setControlled( FaceTools::FaceControl*, bool) override;
   
protected:
    bool doAction() override;

private slots:
    void checkEnable();
    void checkHighlightLandmark( const QPoint&);

private:
    const QIcon _icon;
    const QString _dname;
    FaceTools::FaceControl* _fcont;
    FaceTools::Landmarks::Landmark* _lmk;
    boost::unordered_set<FaceTools::FaceControl*> _fconts;
    void checkActionAdd( FaceTools::FaceControl*);
    void checkHighlight( FaceTools::FaceControl*);
    void actionSingle( FaceTools::FaceControl*);
};  // end class

}   // end namespace

#endif


