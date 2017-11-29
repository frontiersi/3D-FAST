#include <OptionsUpdater.h>
#include <VisualisationAction.h>
#include <FaceControl.h>    // FaceTools
#include <LegendRange.h>    // FaceTools
#include <InteractiveModelViewer.h>
#include <cassert>
using FaceApp::OptionsUpdater;
using FaceApp::OptionsDialog;
using FaceTools::ModelOptions;
using FaceTools::FaceControl;
using FaceTools::LegendRange;

OptionsUpdater::OptionsUpdater( OptionsDialog* dialog)
    : FaceTools::FaceAction(), _dialog(dialog)
{
    connect( _dialog, &OptionsDialog::onOptionsChanged, this, &OptionsUpdater::process);
}   // end ctor


void OptionsUpdater::addController( FaceControl* fcont)
{
    ModelOptions opts;
    updateOptionsFromDialog( opts);
    setControlled( fcont, true);
}   // end addController


void OptionsUpdater::removeController( FaceControl *fcont)
{
    setControlled( fcont, false);
}   // end removeController


void OptionsUpdater::setControlled( FaceControl* fcont, bool enable)
{
    _fconts.erase(fcont);
    assert( fcont);
    fcont->disconnect(this);
    if ( enable)
    {
        _fconts.insert(fcont);
        connect( fcont, &FaceControl::viewUpdated, [=](){ this->setOptionsDialogFromFaceControl( fcont);});
        setOptionsDialogFromFaceControl( fcont);
    }   // end if
}   // end setControlled


// protected
bool OptionsUpdater::doAction()    // Called through process whenever options changed (dialog emits onOptionsChanged)
{
    ModelOptions opts;
    updateOptionsFromDialog( opts);
    foreach ( FaceControl* fcont, _fconts) // Apply to all of the currently controlled views
        fcont->setOptions(opts);

    // Apply options to all of the viewers
    foreach ( FaceTools::ModelViewer* viewer, _viewers)
    {
        viewer->showAxes( opts.showAxes);
        viewer->updateRender();
    }   // end foreach
    return true;
}   // end doAction


// private
void OptionsUpdater::setOptionsDialogFromFaceControl( FaceControl *fcont)
{
    const LegendRange* legend = fcont->getLegend();
    const FaceTools::VisualisationAction* curvis = fcont->getVisualisation();
    if ( curvis && legend->gotMapping( curvis->getDisplayName().toStdString()))
    {
        QColor c0, c1, c2;
        legend->getColours( c0, c1, c2);
        const std::pair<float,float>& minmax = *legend->getRange();
        const std::pair<float,float>& crng = *legend->getCurrent();
        _dialog->setMinMidMaxColours( c0, c1, c2);
        _dialog->setMinMaxLookup( minmax.first, minmax.second, crng.first, crng.second, legend->getNumColours());
    }   // end if
}   // end setOptionsDialogFromFaceControl


// private
void OptionsUpdater::updateOptionsFromDialog( ModelOptions& opts) const
{
    opts.model.backfaceCulling = _dialog->getBackfaceCulling();
    opts.model.vertexSize = _dialog->getModelVertexSize();
    opts.model.lineWidth = _dialog->getModelLineWidth();

    opts.model.surfaceColourMin = _dialog->getLegendMinColour();
    opts.model.surfaceColourMid = _dialog->getModelFlatSurfaceColour();
    opts.model.surfaceColourMax = _dialog->getLegendMaxColour();
    opts.model.opacity = _dialog->getOpacity();

    opts.model.minVisibleScalar = _dialog->getLegendMinLookupVal();
    opts.model.maxVisibleScalar = _dialog->getLegendMaxLookupVal();
    opts.model.numSurfaceColours = _dialog->getLegendNumLookupColours();

    opts.landmarks.radius = _dialog->getLandmarkBaseRadius();
    opts.landmarks.colour = _dialog->getLandmarkBaseColour();

    opts.boundary.cropFactor = _dialog->getFaceCropFactor();

    opts.munits = _dialog->getMeasurementUnits();
    opts.textColour = _dialog->getTextColour();
    opts.showCaptions = _dialog->getShowCaptions();
    opts.showAxes = _dialog->getShowAxes();

    opts.maxTriangleArea = 0.8;
    opts.smoothFactor = 0.8;
}   // end updateOptionsFromDialog

