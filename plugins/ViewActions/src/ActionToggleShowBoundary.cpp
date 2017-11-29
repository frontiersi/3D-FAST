#include <ActionToggleShowBoundary.h>
#include <FaceTools.h>
#include <FaceModel.h>
using FaceApp::ActionToggleShowBoundary;
using FaceTools::FaceControl;

ActionToggleShowBoundary::ActionToggleShowBoundary( const std::string& fname)
    : FaceTools::FaceAction(),
      _icon( fname.c_str())
{
    init();
    setCheckable(true, false);
    setEnabled(false);
}   // end ctor


void ActionToggleShowBoundary::setControlled( FaceControl* fcont, bool enable)
{
    assert(fcont);
    fcont->disconnect( this);
    _fconts.erase(fcont);
    if ( enable)
    {
        checkActionAdd( fcont);
        connect( fcont, &FaceControl::metaUpdated, this, &ActionToggleShowBoundary::checkEnable);
        connect( fcont, &FaceControl::viewUpdated, this, &ActionToggleShowBoundary::checkEnable);
    }   // end else
    setEnabled( !_fconts.empty());
    if ( _fconts.empty())
        setChecked(false);
}   // end setControlled


bool ActionToggleShowBoundary::doAction()
{
    assert(!_fconts.empty());
    foreach ( FaceControl* fcont, _fconts)
        fcont->showBoundary( isChecked());
    return true;
}   // end doAction


// private slot
void ActionToggleShowBoundary::checkEnable()
{
    checkActionAdd( qobject_cast<FaceControl*>(sender()));
    setEnabled( !_fconts.empty());
}   // end checkEnable


// private
void ActionToggleShowBoundary::checkActionAdd( FaceControl* fcont)
{
    assert(fcont);
    setChecked( fcont->isBoundaryShown());
    _fconts.erase(fcont);
    if ( FaceTools::hasReqLandmarks( fcont->getModel()->getObjectMeta()))
        _fconts.insert(fcont);
}   // end checkActionAdd

