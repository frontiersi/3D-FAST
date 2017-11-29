#include <ActionMoveLandmarks.h>
#include <InteractiveModelViewer.h>
#include <FaceModel.h>
#include <FaceView.h>
using FaceApp::ActionMoveLandmarks;
typedef FaceTools::FaceControl FC;

// public
ActionMoveLandmarks::ActionMoveLandmarks( const std::string& dname, const std::string& fname)
    : FaceTools::FaceAction(),
      _icon( fname.c_str()), _dname(dname.c_str()), _lmk(NULL)
{
    init();
    setCheckable(true, false);
    setEnabled(false);
}   // end ctor


// public
void ActionMoveLandmarks::setControlled( FC* fc, bool enable)
{
    setChecked(false);
    _fconts.erase(fc);
    assert(fc);
    fc->disconnect( this);
    if ( enable)
    {
        _fconts.insert( fc);
        connect( fc, &FC::viewUpdated, this, &ActionMoveLandmarks::checkEnable);
        connect( fc, &FC::metaUpdated, this, &ActionMoveLandmarks::checkEnable);
    }   // end if
    setEnabled( _fconts.size() == 1 && (*_fconts.begin())->areLandmarksShown());
}   // end setControlled


// protected slot
bool ActionMoveLandmarks::doAction()
{
    assert(_fconts.size() == 1);
    FC* fc = *_fconts.begin();
    if ( isChecked())
    {
        connect( fc, &FC::onLeftButtonDown, this, &ActionMoveLandmarks::selectLandmark);
        connect( fc, &FC::onLeftButtonUp, this, &ActionMoveLandmarks::deselectLandmark);
    }   // end if
    else
    {
        disconnect( fc, &FC::onLeftButtonDown, this, &ActionMoveLandmarks::selectLandmark);
        disconnect( fc, &FC::onLeftButtonUp, this, &ActionMoveLandmarks::deselectLandmark);
    }   // end else
    return true;
}   // end doAction


// private slot
void ActionMoveLandmarks::checkEnable()
{
    setEnabled( _fconts.size() == 1 && (*_fconts.begin())->areLandmarksShown());
    if ( !isEnabled())
    {
        setChecked(false);
        if ( _fconts.size() == 1)
            doAction();
    }   // end if
}   // end checkEnable


// private slot
void ActionMoveLandmarks::selectLandmark( const QPoint& p)
{
    assert( isChecked());
    assert(_fconts.size() == 1);
    FC* fc = *_fconts.begin();
    _lmk = fc->getModel()->getObjectMeta()->getLandmarkMeta( fc->isLandmarkPointedAt());
    if ( _lmk && _lmk->movable)
    {
        fc->setCameraLocked(true);
        connect( fc, &FC::onLeftDrag, this, &ActionMoveLandmarks::moveLandmark);
    }   // end if
}   // end selectLandmark


// private slot
void ActionMoveLandmarks::deselectLandmark( const QPoint& p)
{
    if ( _lmk)
    {
        assert(_fconts.size() == 1);
        FC* fc = *_fconts.begin();
        fc->setCameraLocked(false);
        cv::Vec3f v;
        fc->getViewer()->calcSurfacePosition( fc->getView()->getActor(), p, v);
        fc->updateLandmark( _lmk->name, &v, true);
        disconnect( fc, &FC::onLeftDrag, this, &ActionMoveLandmarks::moveLandmark);
    }   // end if
    _lmk = NULL;
}   // end deselectLandmark


// private slot
void ActionMoveLandmarks::moveLandmark( const QPoint& p)
{
    assert( isChecked());
    assert(_fconts.size() == 1);
    FC* fc = *_fconts.begin();
    assert( _lmk);
    assert( _lmk->movable);
    cv::Vec3f v;
    fc->getViewer()->calcSurfacePosition( fc->getView()->getActor(), p, v);
    fc->updateLandmark( _lmk->name, &v, false);
}   // end moveLandmark
