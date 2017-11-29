#include <CoreDialogs.h>
#include <VisualisationAction.h>    // FaceTools
#include <LegendRange.h>            // FaceTools
using FaceApp::CoreDialogs;


CoreDialogs::CoreDialogs() :
    _fcont(NULL),
    _helpDialog( new FaceApp::HelpDialog),
    _aboutDialog( new FaceApp::AboutDialog),
    _optionsDialog( new FaceApp::OptionsDialog),
    _licensesDialog( new FaceApp::LicensesDialog),
    _landmarksDialog( new FaceApp::LandmarksDialog),
    _modelInfoDialog( new FaceApp::ModelInfoDialog),
    _pluginsDialog( new QTools::PluginsDialog)
{
    setActive(NULL); // Ensure no model set initially
    connect( _optionsDialog, &FaceApp::OptionsDialog::onOptionsChanged, this, &CoreDialogs::doOptionsChanged);
}   // end ctor


CoreDialogs::~CoreDialogs()
{
    delete _modelInfoDialog;
    delete _landmarksDialog;
    delete _licensesDialog;
    delete _pluginsDialog;
    delete _optionsDialog;
    delete _aboutDialog;
    delete _helpDialog;
}   // end dtor


// public
bool CoreDialogs::loadOptions( const std::string& fname)
{
    return _optionsDialog->load( fname);
}   // end loadOptions


// public
bool CoreDialogs::saveOptions( const std::string& fname)
{
    return _optionsDialog->save( fname);
}   // end saveOptions


// public
void CoreDialogs::hideAll()
{
    _helpDialog->hide();
    _aboutDialog->hide();
    _optionsDialog->hide();
    _pluginsDialog->hide();
    _licensesDialog->hide();
    _landmarksDialog->hide();
    _modelInfoDialog->hide();
}   // end hideAll


// public slot
void CoreDialogs::setActive( FaceTools::FaceControl* fcont)
{
    _landmarksDialog->setModel(fcont);
    if ( _fcont)
    {
        _fcont->disconnect(this);
        _fcont = NULL;
    }   // end if

    if ( fcont)
    {
        setOptionsDialogFromFaceControl( fcont);
        _modelInfoDialog->setModel( fcont->getModel());
        connect( fcont, &FaceTools::FaceControl::viewUpdated, [=](){ this->setOptionsDialogFromFaceControl( fcont);});
        _fcont = fcont;
    }   // end if
    else
        _modelInfoDialog->setModel(NULL);
}   // end setActive


// private
void CoreDialogs::setOptionsDialogFromFaceControl( FaceTools::FaceControl *fcont)
{
    static int i = 0;
    const FaceTools::LegendRange* legend = fcont->getLegend();
    assert( legend);
    const FaceTools::VisualisationAction* curvis = fcont->getVisualisation();
    assert( curvis);
    if ( curvis && legend->gotMapping( curvis->getDisplayName().toStdString()))
    {
        QColor c0, c1, c2;
        legend->getColours( c0, c1, c2);
        const std::pair<float,float>& minmax = *legend->getRange();
        const std::pair<float,float>& crng = *legend->getCurrent();
        _optionsDialog->setMinMidMaxColours( c0, c1, c2);
        _optionsDialog->setMinMaxLookup( minmax.first, minmax.second, crng.first, crng.second, legend->getNumColours());
    }   // end if
}   // end setOptionsDialogFromFaceControl


// private slot
void CoreDialogs::doOptionsChanged()
{
    emit onOptionsChanged( getOptions());
}   // end doOptionsChanged


// public
FaceTools::ModelOptions CoreDialogs::getOptions() const
{
    FaceTools::ModelOptions opts;

    opts.model.backfaceCulling = _optionsDialog->getBackfaceCulling();
    opts.model.vertexSize = _optionsDialog->getModelVertexSize();
    opts.model.lineWidth = _optionsDialog->getModelLineWidth();

    opts.model.surfaceColourMin = _optionsDialog->getLegendMinColour();
    opts.model.surfaceColourMid = _optionsDialog->getModelFlatSurfaceColour();
    opts.model.surfaceColourMax = _optionsDialog->getLegendMaxColour();
    opts.model.opacity = _optionsDialog->getOpacity();

    opts.model.minVisibleScalar = _optionsDialog->getLegendMinLookupVal();
    opts.model.maxVisibleScalar = _optionsDialog->getLegendMaxLookupVal();
    opts.model.numSurfaceColours = _optionsDialog->getLegendNumLookupColours();

    opts.landmarks.radius = _optionsDialog->getLandmarkBaseRadius();
    opts.landmarks.colour = _optionsDialog->getLandmarkBaseColour();

    opts.boundary.cropFactor = _optionsDialog->getFaceCropFactor();

    opts.munits = _optionsDialog->getMeasurementUnits();
    opts.textColour = _optionsDialog->getTextColour();
    opts.showCaptions = _optionsDialog->getShowCaptions();
    opts.showAxes = _optionsDialog->getShowAxes();

    opts.maxTriangleArea = 0.8;
    opts.smoothFactor = 0.8;

    return opts;
}   // end getOptions
