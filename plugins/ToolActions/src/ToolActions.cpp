#include <ToolActions.h>
#include <ActionAddLandmark.h>
#include <ActionDeleteLandmark.h>
#include <ActionCropFace.h>
#include <ActionDetectFace.h>
#include <ActionToggleCalipers.h>
#include <ActionMoveLandmarks.h>
using FaceApp::ToolActions;


ToolActions::ToolActions()
    : FaceTools::FaceActionGroup()
{
    _actions.push_back( new FaceApp::ActionDetectFace("Detect Face", ":/icons/detect_face.png"));
    _actions.push_back( new FaceApp::ActionCropFace("Crop Face", ":/icons/crop_face.png"));
    _actions.push_back( new FaceApp::ActionToggleCalipers("Use Calipers", ":/icons/calipers.png"));
    _actions.push_back( new FaceApp::ActionMoveLandmarks("Move Landmarks", ":/icons/move_landmarks.png"));
    _actions.push_back( new FaceApp::ActionAddLandmark("Add Landmark", ":/icons/add_landmark.png"));
    _actions.push_back( new FaceApp::ActionDeleteLandmark("Delete Landmark", ":/icons/delete_landmark.png"));

    foreach ( FaceTools::FaceAction* action, _actions)
        addAction(action);
}   // end ctor


void ToolActions::initFaceDetection( const QString& haarModelsDir, const QString& faceLmksShapeModel)
{
    qobject_cast<FaceApp::ActionDetectFace*>(_actions[0])->initFaceDetection( haarModelsDir, faceLmksShapeModel);
}   // end initFaceDetection


void ToolActions::addTo( QMenu* menu) const
{
    menu->addAction( _actions[0]->qaction());
    menu->addAction( _actions[1]->qaction());
    menu->addAction( _actions[2]->qaction());
    menu->addAction( _actions[3]->qaction());
}   // end addTo


void ToolActions::addTo( QToolBar* toolbar) const
{
    toolbar->addAction( _actions[2]->qaction());
    toolbar->addAction( _actions[3]->qaction());
    toolbar->addAction( _actions[0]->qaction());
    toolbar->addAction( _actions[1]->qaction());
}   // end addTo


void ToolActions::addToContext( QMenu* cmenu) const
{
    cmenu->addAction( _actions[4]->qaction());
    cmenu->addAction( _actions[5]->qaction());
}   // end addToContext

