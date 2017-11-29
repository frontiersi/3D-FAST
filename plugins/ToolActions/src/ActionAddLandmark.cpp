#include <ActionAddLandmark.h>
#include <InteractiveModelViewer.h>
#include <FaceControl.h>
#include <ObjMetaData.h>
#include <FaceModel.h>
#include <FaceView.h>
#include <QInputDialog>
#include <QMessageBox>
#include <cassert>
using FaceApp::ActionAddLandmark;
using FaceTools::FaceControl;

ActionAddLandmark::ActionAddLandmark( const std::string& dname, const std::string& fname)
    : FaceTools::FaceAction(), _icon( fname.c_str()), _dname(dname.c_str()), _fcont(NULL)
{
    init();
    checkEnable();
}   // end ctor


void ActionAddLandmark::setControlled( FaceControl* fcont, bool enable)
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
        connect( fcont, &FaceControl::onEnteringLandmark, this, &ActionAddLandmark::checkEnable);
        connect( fcont, &FaceControl::onExitingLandmark, this, &ActionAddLandmark::checkEnable);
        connect( fcont, &FaceControl::onEnteringModel, this, &ActionAddLandmark::checkEnable);
        connect( fcont, &FaceControl::onExitingModel, this, &ActionAddLandmark::checkEnable);
    }   // end else
    checkEnable();
}   // end setControlled


bool ActionAddLandmark::doAction()
{
    assert(_fcont);
    assert(_fcont->getViewer());

    // Get a new name for the landmark that isn't one of the existing ones!
    const FaceTools::ObjMetaData::Ptr objmeta = _fcont->getModel()->getObjectMeta();
    boost::unordered_set<std::string> lmnames;
    objmeta->getLandmarks( lmnames);

    // Get new landmark name from user
    bool ok = false;
    std::string newname;
    do
    {
        const QString qname = QInputDialog::getText( NULL, tr("Add new landmark"), tr("Name of new landmark:"),
                                                     QLineEdit::Normal, "", &ok);
        newname = qname.toStdString();
        if ( lmnames.count(newname) > 0)
        {
            ok = false;
            QMessageBox mb( tr("Duplicate Name"), tr(("Landmark '" + newname + "' already exists! Try again?").c_str()),
                            QMessageBox::Information,
                            QMessageBox::Yes | QMessageBox::Default,
                            QMessageBox::No | QMessageBox::Escape,
                            QMessageBox::NoButton);
            if ( mb.exec() == QMessageBox::No)
                newname = "";
        }   // end if
    } while ( !ok && !newname.empty());

    bool added = false;
    if ( ok && !newname.empty())
    {
        added = true;
        cv::Vec3f v;
        const QPoint& p = _fcont->getMouseCoords();
        const bool onModel = _fcont->getViewer()->calcSurfacePosition( _fcont->getView()->getSurfaceActor(), p, v);
        assert(onModel);
        _fcont->updateLandmark( newname, &v);
        _fcont->showLandmark( true, newname);
    }   // end if
    return added;
}   // end doAction


// private slot
void ActionAddLandmark::checkEnable()
{
    if ( !_fcont)
        setEnabled(false);
    else
    {
        // Allow landmark add if pointing at model but not an existing landmark
        const bool onLandmark = !_fcont->isLandmarkPointedAt().empty();
        setEnabled( _fcont->isPointedAt() && !onLandmark);
    }   // end else
}   // end checkEnable


