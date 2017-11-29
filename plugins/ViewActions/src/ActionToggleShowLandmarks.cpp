#include <ActionToggleShowLandmarks.h>
#include <InteractiveModelViewer.h>
#include <ModelOptions.h>
#include <FaceModel.h>
using FaceApp::ActionToggleShowLandmarks;
using FaceTools::FaceControl;


ActionToggleShowLandmarks::ActionToggleShowLandmarks( const std::string& dname, const std::string& fname)
    : FaceTools::FaceAction(), _icon(fname.c_str()), _dname(dname.c_str()), _fcont(NULL), _lmk(NULL)
{
    init();
    setCheckable(true, false);
    setEnabled(false);
}   // end ctor


void ActionToggleShowLandmarks::setControlled( FaceControl* fcont, bool enable)
{
    assert(fcont);
    fcont->disconnect(this);
    _fconts.erase(fcont);

    if ( enable)
    {
        checkActionAdd( fcont);
        connect( fcont, &FaceControl::metaUpdated, this, &ActionToggleShowLandmarks::checkEnable);
        connect( fcont, &FaceControl::viewUpdated, this, &ActionToggleShowLandmarks::checkEnable);
        actionSingle(fcont);
    }   // end if
    else
        fcont->showLandmarks(false);

    setEnabled( !_fconts.empty());
    if ( _fconts.empty())
        setChecked(false);
}   // end setControlled


void ActionToggleShowLandmarks::actionSingle( FaceControl* fcont)
{
    fcont->showLandmarks( isChecked());
    if ( isChecked())
        connect( fcont, &FaceControl::onMouseMove, this, &ActionToggleShowLandmarks::checkHighlightLandmark);
    else
        disconnect( fcont, &FaceControl::onMouseMove, this, &ActionToggleShowLandmarks::checkHighlightLandmark);
    checkHighlight( fcont);
}   // end actionSingle


bool ActionToggleShowLandmarks::doAction()
{
    foreach ( FaceControl* fcont, _fconts)
        actionSingle(fcont);
    return true;
}   // end doAction


// private slot
void ActionToggleShowLandmarks::checkEnable()
{
    checkActionAdd( qobject_cast<FaceControl*>(sender()));
    setEnabled( !_fconts.empty());
    doAction();
}   // end checkEnable


// private
void ActionToggleShowLandmarks::checkActionAdd( FaceControl* fcont)
{
    assert(fcont);
    if ( fcont->areLandmarksShown())
        setChecked( true);
    _fconts.erase(fcont);
    if ( fcont->getModel()->getObjectMeta()->hasLandmarks())
        _fconts.insert(fcont);
}   // end checkActionAdd


// private slot
void ActionToggleShowLandmarks::checkHighlightLandmark( const QPoint&)
{
    FaceControl* fcont = qobject_cast<FaceControl*>( sender());
    if ( fcont)
        checkHighlight( fcont);
}   // end checkHighlightLandmark


// private
void ActionToggleShowLandmarks::checkHighlight( FaceTools::FaceControl* fcont)
{
    assert(fcont);
    FaceTools::Landmarks::Landmark* nlmk = fcont->getModel()->getObjectMeta()->getLandmarkMeta( fcont->isLandmarkPointedAt());

    if ( _lmk && (_lmk != nlmk || _fcont != fcont))  // Remove the old caption
        _fcont->highlightLandmark( false, _lmk->name);

    _fcont = fcont;
    _lmk = nlmk;
    if ( _lmk)
        fcont->highlightLandmark( true, _lmk->name);

    _fcont->getViewer()->updateRender();
}   // end checkHighlight

