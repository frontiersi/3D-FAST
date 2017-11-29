#ifndef FACE_APP_ACTION_TOGGLE_SHOW_BOUNDARY_H
#define FACE_APP_ACTION_TOGGLE_SHOW_BOUNDARY_H

#include <FaceActionInterface.h>
#include <FaceControl.h>

namespace FaceApp {

class ActionToggleShowBoundary : public FaceTools::FaceAction
{ Q_OBJECT
public:
    explicit ActionToggleShowBoundary( const std::string& iconfilename);

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return "Show Boundary";}

    void setControlled( FaceTools::FaceControl*, bool) override;

protected:
    bool doAction() override;

private slots:
    void checkEnable();

private:
    const QIcon _icon;
    boost::unordered_set<FaceTools::FaceControl*> _fconts;
    void checkActionAdd( FaceTools::FaceControl*);
};  // end class

}   // end namespace

#endif


