#include <ViewActions.h>
#include <ActionToggleShowBoundary.h>
#include <ActionToggleShowLandmarks.h>
#include <ActionOrientCameraToFace.h>   // FaceTools
#include <ActionSetFocus.h>             // FaceTools
using FaceApp::ViewActions;


ViewActions::ViewActions()
    : FaceTools::FaceActionGroup()
{
    _actions.push_back( new FaceApp::ActionToggleShowLandmarks("Show Landmarks", ":/icons/landmarks.png"));
    _actions.push_back( new FaceApp::ActionToggleShowBoundary(":/icons/boundary.png"));
    _actions.push_back( new FaceTools::ActionOrientCameraToFace("Orient Camera to Face", ":/icons/orient_camera.png"));
    _actions.push_back( new FaceTools::ActionSetFocus());

    foreach ( FaceTools::FaceAction* action, _actions)
        addAction(action);
}   // end ctor


void ViewActions::addTo( QMenu* menu) const
{
    menu->addAction( _actions[0]->qaction());
    menu->addAction( _actions[1]->qaction());
    menu->addAction( _actions[2]->qaction());
}   // end addTo


void ViewActions::addTo( QToolBar* toolbar) const
{
    toolbar->addAction( _actions[0]->qaction());
    toolbar->addAction( _actions[1]->qaction());
    toolbar->addAction( _actions[2]->qaction());
}   // end addTo


void ViewActions::addToContext( QMenu* cmenu) const
{
    cmenu->addAction( _actions[0]->qaction());  // ShowLandmarks
    cmenu->addAction( _actions[1]->qaction());  // ShowBoundary
    cmenu->addAction( _actions[2]->qaction());  // OrientCameraToFace
    cmenu->addAction( _actions[3]->qaction());  // SetFocus
}   // end addToContext
