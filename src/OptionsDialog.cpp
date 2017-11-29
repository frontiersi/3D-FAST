#include <OptionsDialog.h>
#include <ui_OptionsDialog.h>
#include <QColorDialog>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <TwoHandleSlider.h>
#include <algorithm>
using FaceApp::OptionsDialog;


OptionsDialog::OptionsDialog( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->colourRangeSliderLayout->addWidget( new QTools::TwoHandleSlider(this, false));

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    setWindowFlags( Qt::WindowCloseButtonHint);

    // Dynamic options need to inform the rest of the app that they've changed so the visualisation
    // elements can react to the new parameter settings immediately.
    connect( ui->landmarkBaseRadiusSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
             this, &OptionsDialog::onOptionsChanged);
    connect( ui->faceCropFactorDoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
             this, &OptionsDialog::onOptionsChanged);

    connect( ui->measurementUnitsLineEdit, &QLineEdit::editingFinished,
             this, &OptionsDialog::onOptionsChanged);
    connect( ui->textColourSelectButton, &QPushButton::clicked,
             this, &OptionsDialog::doClickedSetTextColour);

    connect( ui->modelLineWidthSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
             this, &OptionsDialog::onOptionsChanged);
    connect( ui->modelVertexSizeSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
             this, &OptionsDialog::onOptionsChanged);
    connect( ui->numLookupColoursSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
             this, &OptionsDialog::onOptionsChanged);

    connect( ui->backfaceCullingCheckBox, &QCheckBox::clicked, this, &OptionsDialog::onOptionsChanged);
    connect( ui->showCaptionsCheckBox, &QCheckBox::clicked, this, &OptionsDialog::onOptionsChanged);
    connect( ui->showAxesCheckBox, &QCheckBox::clicked, this, &OptionsDialog::onOptionsChanged);

    QTools::TwoHandleSlider* colourRangeSlider = qobject_cast<QTools::TwoHandleSlider*>(ui->colourRangeSliderLayout->itemAt(0)->widget());
    connect( colourRangeSlider, &QTools::TwoHandleSlider::value0Changed, this, &OptionsDialog::onOptionsChanged);
    connect( colourRangeSlider, &QTools::TwoHandleSlider::value1Changed, this, &OptionsDialog::onOptionsChanged);

    connect( ui->opacitySlider, &QSlider::valueChanged, this, &OptionsDialog::onOptionsChanged);

    // Slots open colour chooser dialogs
    connect( ui->landmarkBaseColourSelectButton, &QPushButton::clicked,
             this, &OptionsDialog::doClickedSetLandmarkBaseColour);

    connect( ui->modelFlatSurfaceColourSelectButton, &QPushButton::clicked,
             this, &OptionsDialog::doClickedSetModelFlatSurfaceColour);

    connect( ui->minLegendColourSelectButton, &QPushButton::clicked,
             this, &OptionsDialog::doClickedSetLegendMinColour);
    connect( ui->maxLegendColourSelectButton, &QPushButton::clicked,
             this, &OptionsDialog::doClickedSetLegendMaxColour);

    // Accept or reject changes
    connect( ui->buttonBox, &QDialogButtonBox::clicked, this, &OptionsDialog::clickedButtonBox);

    acceptCurrentValues();  // Initialises accepted state
}   // end ctor


OptionsDialog::~OptionsDialog()
{
    delete ui;
}   // end dtor


void OptionsDialog::setMinMaxLookup( float minv, float maxv, float v0, float v1, size_t ncols)
{
    QTools::TwoHandleSlider* slider = qobject_cast<QTools::TwoHandleSlider*>(ui->colourRangeSliderLayout->itemAt(0)->widget());
    assert( minv <= maxv);
    assert( v0 <= v1);
    assert( v0 >= minv);
    assert( v0 <= maxv);
    assert( v1 >= minv);
    assert( v1 <= maxv);
    slider->resetRange( minv, maxv);
    slider->setValue0(v0);
    slider->setValue1(v1);
    ncols = std::max<size_t>( 2, std::min<size_t>( 256, ncols));
    ui->numLookupColoursSpinBox->setValue( ncols < 2 ? 2 : (int)ncols);
}   // end setMinMaxLookup


void OptionsDialog::setMinMidMaxColours( const QColor& c0, const QColor& c1, const QColor& c2)
{
    ui->minLegendColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(c0.name()));
    _acceptedLegendMinColour = _curLegendMinColour = c0;
    ui->modelFlatSurfaceColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(c1.name()));
    _acceptedModelFlatSurfaceColour = _curModelFlatSurfaceColour = c1;
    ui->maxLegendColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(c2.name()));
    _acceptedLegendMaxColour = _curLegendMaxColour = c2;
}   // end setMinMidMaxColours


float OptionsDialog::getLandmarkBaseRadius() { return float(ui->landmarkBaseRadiusSpinBox->value());}
const QColor& OptionsDialog::getLandmarkBaseColour() { return _curLandmarkBaseColour;}

double OptionsDialog::getFaceCropFactor() { return ui->faceCropFactorDoubleSpinBox->value();}

int OptionsDialog::getModelLineWidth() { return ui->modelLineWidthSpinBox->value();}
int OptionsDialog::getModelVertexSize() { return ui->modelVertexSizeSpinBox->value();}
const QColor& OptionsDialog::getModelFlatSurfaceColour() { return _curModelFlatSurfaceColour;}

float OptionsDialog::getLegendMinLookupVal() { return qobject_cast<QTools::TwoHandleSlider*>(ui->colourRangeSliderLayout->itemAt(0)->widget())->value0();}
float OptionsDialog::getLegendMaxLookupVal() { return qobject_cast<QTools::TwoHandleSlider*>(ui->colourRangeSliderLayout->itemAt(0)->widget())->value1();}

const QColor& OptionsDialog::getLegendMinColour() { return _curLegendMinColour;}
const QColor& OptionsDialog::getLegendMaxColour() { return _curLegendMaxColour;}
int OptionsDialog::getLegendNumLookupColours() { return ui->numLookupColoursSpinBox->value();}
double OptionsDialog::getOpacity() const { return double(ui->opacitySlider->value())/ui->opacitySlider->maximum();}

std::string OptionsDialog::getMeasurementUnits() { return ui->measurementUnitsLineEdit->text().toStdString();}
const QColor& OptionsDialog::getTextColour() { return _curTextColour;}
bool OptionsDialog::getBackfaceCulling() { return ui->backfaceCullingCheckBox->checkState() == Qt::Checked;}
bool OptionsDialog::getShowCaptions() { return ui->showCaptionsCheckBox->checkState() == Qt::Checked;}
bool OptionsDialog::getShowAxes() { return ui->showAxesCheckBox->checkState() == Qt::Checked;}


namespace
{

std::string LANDMARK_BASE_RADIUS = "LANDMARK_BASE_RADIUS";
std::string LANDMARK_BASE_COLOUR = "LANDMARK_BASE_COLOUR";

std::string FACE_CROP_FACTOR = "FACE_CROP_FACTOR";

std::string MODEL_LINE_WIDTH = "MODEL_LINE_WIDTH";
std::string MODEL_VERTEX_SIZE = "MODEL_VERTEX_SIZE";
std::string MODEL_FLAT_SURFACE_COLOUR = "MODEL_FLAT_SURFACE_COLOUR";

std::string LEGEND_MIN_LOOKUP_VAL = "LEGEND_MIN_LOOKUP_VAL";
std::string LEGEND_MAX_LOOKUP_VAL = "LEGEND_MAX_LOOKUP_VAL";
std::string LEGEND_MIN_COLOUR = "LEGEND_MIN_COLOUR";
std::string LEGEND_MAX_COLOUR = "LEGEND_MAX_COLOUR";
std::string LEGEND_NUM_LOOKUP_COLOURS = "LEGEND_NUM_LOOKUP_COLOURS";

std::string MEASUREMENT_UNITS = "MEASUREMENT_UNITS";
std::string TEXT_COLOUR = "TEXT_COLOUR";
std::string BACKFACE_CULLING = "BACKFACE_CULLING";
std::string SHOW_CAPTIONS = "SHOW_CAPTIONS";


QColor readColour( std::istream& iss)
{
    QRgb rgba;
    iss >> rgba;
    return QColor::fromRgba(rgba);
}   // end readColour

}   // end namespace



// public slot
bool OptionsDialog::load( const std::string& prefsfile)
{
    std::ifstream ifs( prefsfile.c_str());
    if ( !ifs.is_open())
    {
        std::cerr << "[ERROR] OptionsDialog::load: Unable to open preferences files " << prefsfile << "!" << std::endl;
        return false;
    }   // end if

    bool readFile = false;
    try
    {
        std::string ln;
        while ( std::getline(ifs, ln))
        {
            if ( ln.empty() || ln[0] == '#')
                continue;

            std::string tok, nullVal;
            std::istringstream iss(ln);
            iss >> tok;
            if ( tok == LANDMARK_BASE_RADIUS)
                iss >> _acceptedLandmarkBaseRadius;
            else if ( tok == LANDMARK_BASE_COLOUR)
                _acceptedLandmarkBaseColour = readColour(iss);
            else if ( tok == FACE_CROP_FACTOR)
                iss >> _acceptedFaceCropFactor;
            else if ( tok == MODEL_LINE_WIDTH)
                iss >> _acceptedModelLineWidth;
            else if ( tok == MODEL_VERTEX_SIZE)
                iss >> _acceptedModelVertexSize;
            else if ( tok == MODEL_FLAT_SURFACE_COLOUR)
                _acceptedModelFlatSurfaceColour = readColour( iss);
            else if ( tok == LEGEND_MIN_LOOKUP_VAL)
                iss >> _acceptedLegendMinLookupVal;
            else if ( tok == LEGEND_MAX_LOOKUP_VAL)
                iss >> _acceptedLegendMaxLookupVal;
            else if ( tok == LEGEND_MIN_COLOUR)
                _acceptedLegendMinColour = readColour( iss);
            else if ( tok == LEGEND_MAX_COLOUR)
                _acceptedLegendMaxColour = readColour( iss);
            else if ( tok == LEGEND_NUM_LOOKUP_COLOURS)
                iss >> _acceptedLegendNumLookupColours;
            else if ( tok == BACKFACE_CULLING)
                iss >> _acceptedBackfaceCulling;
            else if ( tok == SHOW_CAPTIONS)
                iss >> _acceptedShowCaptions;
            else if ( tok == MEASUREMENT_UNITS)
                iss >> _acceptedMeasurementUnits;
            else if ( tok == TEXT_COLOUR)
                _acceptedTextColour = readColour( iss);
            else
            {
                std::cerr << "Ignored invalid " << prefsfile << " option '" << tok << "'" << std::endl;
                iss >> nullVal;
            }   // end else
        }   // end while

        ifs.close();
        readFile = true;
        restoreAcceptedValues();
    }   // end try
    catch ( const std::exception& e)
    {
        std::cerr << "[ERROR] OptionsDialog::load: " << e.what() << std::endl;
        readFile = false;
    }   // end catch

    return readFile;
}   // end load


// public slot
bool OptionsDialog::save( const std::string& prefsfile) const
{
    bool savedFile = false;
    std::ofstream ofs( prefsfile.c_str());
    try
    {
        ofs << LANDMARK_BASE_RADIUS << " " << _acceptedLandmarkBaseRadius << std::endl;
        ofs << LANDMARK_BASE_COLOUR << " " << _acceptedLandmarkBaseColour.rgba() << std::endl;

        ofs << FACE_CROP_FACTOR << " " << _acceptedFaceCropFactor << std::endl;

        ofs << MODEL_LINE_WIDTH << " " << _acceptedModelLineWidth << std::endl;
        ofs << MODEL_VERTEX_SIZE << " " << _acceptedModelVertexSize << std::endl;
        ofs << MODEL_FLAT_SURFACE_COLOUR << " " << _acceptedModelFlatSurfaceColour.rgba() << std::endl;

        ofs << LEGEND_MIN_LOOKUP_VAL << " " << _acceptedLegendMinLookupVal << std::endl;
        ofs << LEGEND_MAX_LOOKUP_VAL << " " << _acceptedLegendMaxLookupVal << std::endl;
        ofs << LEGEND_MIN_COLOUR << " " << _acceptedLegendMinColour.rgba() << std::endl;
        ofs << LEGEND_MAX_COLOUR << " " << _acceptedLegendMaxColour.rgba() << std::endl;
        ofs << LEGEND_NUM_LOOKUP_COLOURS << " " << _acceptedLegendNumLookupColours << std::endl;
        ofs << BACKFACE_CULLING << " " << _acceptedBackfaceCulling << std::endl;
        ofs << SHOW_CAPTIONS << " " << _acceptedShowCaptions << std::endl;
        ofs << MEASUREMENT_UNITS << " " << _acceptedMeasurementUnits << std::endl;
        ofs << TEXT_COLOUR << " " << _acceptedTextColour.rgba() << std::endl;

        ofs.close();
        std::cerr << "Options saved to " << prefsfile << std::endl;
        savedFile = true;
    }   // end try
    catch ( const std::exception& e)
    {
        std::cerr << "ERROR: OptionsDialog::save: " << e.what() << std::endl;
        savedFile = false;
    }   // end catch
    return savedFile;
}   // end save


// private
void OptionsDialog::acceptCurrentValues()
{
    _acceptedLandmarkBaseRadius = getLandmarkBaseRadius();
    _acceptedLandmarkBaseColour = getLandmarkBaseColour();

    _acceptedFaceCropFactor = getFaceCropFactor();

    _acceptedModelVertexSize = getModelVertexSize();
    _acceptedModelLineWidth = getModelLineWidth();
    _acceptedModelFlatSurfaceColour = getModelFlatSurfaceColour();

    _acceptedLegendMinLookupVal = getLegendMinLookupVal();
    _acceptedLegendMaxLookupVal = getLegendMaxLookupVal();
    _acceptedLegendMinColour = getLegendMinColour();
    _acceptedLegendMaxColour = getLegendMaxColour();
    _acceptedLegendNumLookupColours = getLegendNumLookupColours();

    _acceptedBackfaceCulling = getBackfaceCulling();
    _acceptedShowCaptions = getShowCaptions();
    _acceptedShowAxes = getShowAxes();
    _acceptedMeasurementUnits = getMeasurementUnits();
    _acceptedTextColour = getTextColour();
}   // end acceptNewValues


// private
void OptionsDialog::restoreAcceptedValues()
{
    ui->landmarkBaseRadiusSpinBox->setValue(_acceptedLandmarkBaseRadius);
    ui->faceCropFactorDoubleSpinBox->setValue(_acceptedFaceCropFactor);

    ui->modelLineWidthSpinBox->setValue(_acceptedModelLineWidth);
    ui->modelVertexSizeSpinBox->setValue(_acceptedModelVertexSize);

    QTools::TwoHandleSlider* colourRangeSlider = qobject_cast<QTools::TwoHandleSlider*>(ui->colourRangeSliderLayout->itemAt(0)->widget());
    colourRangeSlider->setValue0(_acceptedLegendMinLookupVal);
    colourRangeSlider->setValue1(_acceptedLegendMaxLookupVal);
    ui->numLookupColoursSpinBox->setValue(_acceptedLegendNumLookupColours);

    ui->backfaceCullingCheckBox->setChecked( _acceptedBackfaceCulling);
    ui->showCaptionsCheckBox->setChecked( _acceptedShowCaptions);
    ui->showAxesCheckBox->setChecked( _acceptedShowAxes);

    ui->landmarkBaseColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(_acceptedLandmarkBaseColour.name()));

    ui->modelFlatSurfaceColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(_acceptedModelFlatSurfaceColour.name()));
    ui->minLegendColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(_acceptedLegendMinColour.name()));
    ui->maxLegendColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(_acceptedLegendMaxColour.name()));
    ui->textColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(_acceptedTextColour.name()));

    ui->measurementUnitsLineEdit->setText( _acceptedMeasurementUnits.c_str());

    _curLandmarkBaseColour = _acceptedLandmarkBaseColour;
    _curModelFlatSurfaceColour = _acceptedModelFlatSurfaceColour;
    _curLegendMinColour = _acceptedLegendMinColour;
    _curLegendMaxColour = _acceptedLegendMaxColour;
    _curTextColour = _acceptedTextColour;

    emit onOptionsChanged();
}   // end restoreOldValues


// private slot
void OptionsDialog::clickedButtonBox( QAbstractButton* button)
{
    const QDialogButtonBox::ButtonRole brole = ui->buttonBox->buttonRole(button);
    if ( brole == QDialogButtonBox::AcceptRole)
        acceptCurrentValues();
    else    // Cancel
        restoreAcceptedValues();
}   // end clickedButtonBox


// private slot
void OptionsDialog::doClickedSetLandmarkBaseColour()
{
    const QColor newColour = QColorDialog::getColor( _curLandmarkBaseColour, this, "3D-FAST | Choose Landmark Base Colour");
    ui->landmarkBaseColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(newColour.name()));
    _curLandmarkBaseColour = newColour;
    emit onOptionsChanged();
}   // end doClickedSetLandmarkBaseColour


// private slot
void OptionsDialog::doClickedSetModelFlatSurfaceColour()
{
    const QColor c1 = QColorDialog::getColor( _curModelFlatSurfaceColour, this, "3D-FAST | Choose Model / Legend Mid Colour");
    if ( c1.isValid())
    {
        ui->modelFlatSurfaceColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(c1.name()));
        _curModelFlatSurfaceColour = c1;
        emit onOptionsChanged();
    }   // end if
}   // end doClickedSetModelFlatSurfaceColour


// private slot
void OptionsDialog::doClickedSetLegendMinColour()
{
    const QColor c0 = QColorDialog::getColor( _curLegendMinColour, this, "3D-FAST | Choose Model / Legend Min Colour");
    if ( c0.isValid())
    {
        ui->minLegendColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(c0.name()));
        _curLegendMinColour = c0;
        emit onOptionsChanged();
    }   // end if
}   // end doClickedSetLegendMinColour


// private slot
void OptionsDialog::doClickedSetLegendMaxColour()
{
    const QColor c2 = QColorDialog::getColor( _curLegendMaxColour, this, "3D-FAST | Choose Model / Legend Max Colour");
    if ( c2.isValid())
    {
        ui->maxLegendColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(c2.name()));
        _curLegendMaxColour = c2;
        emit onOptionsChanged();
    }   // end if
}   // end doClickedSetLegendMaxColour


// private slot
void OptionsDialog::doClickedSetTextColour()
{
    const QColor newColour = QColorDialog::getColor( _curTextColour, this, "3D-FAST | Choose Text Colour");
    if ( newColour.isValid())
    {
        ui->textColourSelectButton->setStyleSheet( QString("background-color: %1;").arg(newColour.name()));
        _curTextColour = newColour;
        emit onOptionsChanged();
    }   // end if
}   // end doClickedSetTextColour
