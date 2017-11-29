#include <ActionToggleCalipers.h>
#include <InteractiveModelViewer.h>
#include <FaceView.h>
using FaceApp::ActionToggleCalipers;
using FaceTools::SurfacePathDrawer;
using FaceTools::FaceControl;


ActionToggleCalipers::ActionToggleCalipers( const std::string& dname, const std::string& fname)
    : FaceTools::FaceAction(),
      _icon( fname.c_str()), _dname( dname.c_str()), _pdrawer(NULL), _handle(NULL)
{
    init();
    setCheckable(true, false);
    setEnabled(false);
}   // end ctor


ActionToggleCalipers::~ActionToggleCalipers()
{
    destroyPathDrawer();
}   // end dtor


// public
void ActionToggleCalipers::setControlled( FaceControl* fcont, bool enable)
{
    destroyPathDrawer();
    setChecked(false);
    if ( _fconts.size() == 1)
    {
        FaceControl* fcont2 = *_fconts.begin();
        fcont2->disconnect(this);
    }   // end if

    assert(fcont);
    fcont->disconnect( this);
    _fconts.erase(fcont);
    if ( enable)
        _fconts.insert(fcont);

    setEnabled( _fconts.size() == 1);
}   // end setControlled


// private
void ActionToggleCalipers::createPathDrawer()
{
    assert( _fconts.size() == 1);
    assert( _pdrawer == NULL);
    FaceControl* fcont = *_fconts.begin();
    _pdrawer = new SurfacePathDrawer( fcont);
    _pdrawer->setVisible( true);
    _handle = NULL;
}   // end createPathDrawer


// private
void ActionToggleCalipers::destroyPathDrawer()
{
    if ( _pdrawer)
        delete _pdrawer;
    _pdrawer = NULL;
    _handle = NULL;
    if ( _fconts.size() == 1)
    {
        FaceControl* fcont = *_fconts.begin();
        fcont->getViewer()->updateRender();
    }   // end if
}   // end destroyPathDrawer


bool ActionToggleCalipers::doAction()
{
    assert( _fconts.size() == 1);
    FaceControl* fcont = *_fconts.begin();

    if ( isChecked())
    {
        createPathDrawer();
        connect( fcont, &FaceControl::onLeftDrag, this, &ActionToggleCalipers::dragHandle);
        connect( fcont, &FaceControl::onLeftButtonDown, this, &ActionToggleCalipers::selectHandle);
        connect( fcont, &FaceControl::onLeftButtonUp, this, &ActionToggleCalipers::deselectHandle);
        connect( fcont, &FaceControl::onMouseMove, this, &ActionToggleCalipers::doMouseMove);
    }   // end if
    else
    {
        fcont->disconnect(this);
        destroyPathDrawer();
    }   // end else
    return true;
}   // end doAction


// private slot (on left button down)
void ActionToggleCalipers::selectHandle( const QPoint& p)
{
    assert( _fconts.size() == 1);
    FaceControl* fcont = *_fconts.begin();
    assert( _pdrawer);
    _handle = NULL;
    const size_t npoints = _pdrawer->nhandles();
    if ( npoints == 0)  // Set the first endpoint if no points yet set.
    {
        _pdrawer->setHandle0(p);
        fcont->setCameraLocked( true);
    }   // end if
    else if ( npoints == 2)    // Otherwise, selects the current point to drag (if any)
    {
        _handle = _pdrawer->handle(p);
        if ( _handle != 0x0)
            fcont->setCameraLocked( true);
    }   // end if
}   // end selectHandle


// private slot (on left drag)
void ActionToggleCalipers::dragHandle( const QPoint& p)
{
    assert( _fconts.size() == 1);
    assert( _pdrawer);
    FaceControl* fcont = *_fconts.begin();
    assert( fcont->isPointedAt());
    const size_t npoints = _pdrawer->nhandles();
    if (_handle)
        _handle->set(p);
    else if ( npoints < 2)
        _handle = _pdrawer->setHandle1(p);
    fcont->getViewer()->updateRender();
}   // end dragHandle


// private slot (on left button up)
void ActionToggleCalipers::deselectHandle( const QPoint&)
{
    assert( _fconts.size() == 1);
    assert( _pdrawer);
    _handle = NULL;
    FaceControl* fcont = *_fconts.begin();
    fcont->setCameraLocked( false);
}   // end deselectHandle


void ActionToggleCalipers::doMouseMove( const QPoint& p)
{
    assert( _fconts.size() == 1);
    assert( _pdrawer);
    _pdrawer->highlightHandle0(false);
    _pdrawer->highlightHandle1(false);
    SurfacePathDrawer::Handle* handle = _pdrawer->handle(p);
    if ( handle)
        handle->highlight(true);
    FaceControl* fcont = *_fconts.begin();
    fcont->getViewer()->updateRender();
}   // end doMouseMove
