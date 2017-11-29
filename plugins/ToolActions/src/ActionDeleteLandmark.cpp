#include <ActionDeleteLandmark.h>
using FaceApp::ActionDeleteLandmark;
#include <FaceControl.h>
#include <FaceModel.h>
#include <QMessageBox>
#include <cassert>
using FaceTools::FaceControl;


ActionDeleteLandmark::ActionDeleteLandmark( const std::string& dname, const std::string& fname)
    : FaceTools::FaceAction(), _icon( fname.c_str()), _dname(dname.c_str()), _fcont(NULL)
{
    init();
    checkEnable();
}   // end ctor


void ActionDeleteLandmark::setControlled( FaceControl* fcont, bool enable)
{
    if ( _fcont)
    {
        _fcont->disconnect( this);
        _fcont = NULL;
    }   // end if
    assert(fcont);
    fcont->disconnect( this);
    if ( enable)
    {
        _fcont = fcont;
        connect( fcont, &FaceControl::onEnteringLandmark, this, &ActionDeleteLandmark::checkEnable);
        connect( fcont, &FaceControl::onExitingLandmark, this, &ActionDeleteLandmark::checkEnable);
        connect( fcont, &FaceControl::onEnteringModel, this, &ActionDeleteLandmark::checkEnable);
        connect( fcont, &FaceControl::onExitingModel, this, &ActionDeleteLandmark::checkEnable);
        connect( fcont, &FaceControl::viewUpdated, this, &ActionDeleteLandmark::checkEnable);
    }   // end else
    checkEnable();
}   // end setInteractor


bool ActionDeleteLandmark::doAction()
{
    // Check if user really wants to delete the landmark
    QMessageBox mb( tr("Delete Landmark"), tr(("Really delete landmark '" + _delLandmark + "'?").c_str()),
                    QMessageBox::Warning,
                    QMessageBox::Yes,
                    QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
                    QMessageBox::NoButton);
    if ( mb.exec() == QMessageBox::No)
        return false;

    _fcont->updateLandmark( _delLandmark, NULL);
    _delLandmark = "";
    return true;
}   // end doAction


// private slot
void ActionDeleteLandmark::checkEnable()
{
    std::string lm;
    bool canDelete = false;
    if ( _fcont)
    {
        lm = _fcont->isLandmarkPointedAt();
        canDelete = !lm.empty() &&
                    _fcont->areLandmarksShown() &&
                    _fcont->getModel()->getObjectMeta()->getLandmarkMeta(lm)->deletable;
    }   // end if

    _delLandmark = "";
    setText( tr("Delete landmark"));
    if ( canDelete)
    {
        _delLandmark = lm;
        setText( tr(("Delete landmark '" + lm + "'").c_str()));
    }   // end if
    setEnabled( canDelete);
}   // checkEnable

