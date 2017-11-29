#ifndef FACE_APP_ACTION_TOGGLE_CALIPERS_H
#define FACE_APP_ACTION_TOGGLE_CALIPERS_H

#include <FaceActionInterface.h>
#include <SurfacePathDrawer.h>
#include <FaceControl.h>

namespace FaceApp {

class ActionToggleCalipers : public FaceTools::FaceAction
{ Q_OBJECT
public:
    ActionToggleCalipers( const std::string& dname, const std::string& iconfilename);
    ~ActionToggleCalipers() override;

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return _dname;}

    void setControlled( FaceTools::FaceControl*, bool) override;

protected:
    bool doAction() override;

private slots:
    void selectHandle( const QPoint&);
    void deselectHandle( const QPoint&);
    void dragHandle( const QPoint&);
    void doMouseMove( const QPoint&);

private:
    const QIcon _icon;
    const QString _dname;
    boost::unordered_set<FaceTools::FaceControl*> _fconts;
    FaceTools::SurfacePathDrawer* _pdrawer;
    FaceTools::SurfacePathDrawer::Handle* _handle;
    void createPathDrawer();
    void destroyPathDrawer();
};  // end class

}   // end namespace

#endif
