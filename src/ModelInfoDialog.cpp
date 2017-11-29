#include <ModelInfoDialog.h>
#include <ui_ModelInfoDialog.h>
#include <ObjModelIntegrityChecker.h>
#include <QDateTime>
#include <QTimeZone>
#include <boost/filesystem.hpp>
using FaceApp::ModelInfoDialog;
using FaceTools::FaceModel;

// private
ModelInfoDialog::ModelInfoDialog( QWidget *parent)
    : QDialog(parent), ui(new Ui::ModelInfoDialog), _fmodel(NULL)
{
    ui->setupUi(this);
    setWindowFlags( Qt::WindowCloseButtonHint);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}   // end ctor


// private
ModelInfoDialog::~ModelInfoDialog()
{
    delete ui;
}   // end dtor


// private
void ModelInfoDialog::clear()
{
    ui->modelFileLabel->setText( "Import a model");
    ui->dateTimeEdit->setDateTime( QDateTime());
    ui->numGeometryVerticesLabel->setText("0");
    ui->numTextureVerticesLabel->setText("0");
    ui->numPolygonsLabel->setText("0");
    ui->numBoundaryLabel->setText("0");
    ui->numNonFlatLabel->setText("0");
    ui->numUnconnectedLabel->setText("0");
    ui->numFlatJunctionLabel->setText("0");
    ui->numNonFlatJunctionALabel->setText("0");
    ui->numNonFlatJunctionBLabel->setText("0");
}   // end clear


// public
void ModelInfoDialog::setModel( const FaceModel* fmodel)
{
    clear();

    if ( _fmodel)
        _fmodel->disconnect(this);
    _fmodel = fmodel;
    if ( !_fmodel)
        return;

    const std::string& fname = _fmodel->getFilePath();
    const QString filename = boost::filesystem::path( fname).filename().string().c_str();
    ui->modelFileLabel->setText( filename);
    ui->dateTimeEdit->setDateTime( QDateTime::currentDateTime());
    QDateTime dateTime = QDateTime::fromString( boost::filesystem::path( filename.toStdString()).stem().string().c_str(), "yyMMddHHmmss");
    if ( dateTime.isValid())
    {
        dateTime = dateTime.addYears(100);
        dateTime.setTimeSpec( Qt::UTC);
        ui->dateTimeEdit->setDateTime( dateTime.toLocalTime());
    }   // end if

    const RFeatures::ObjModel::Ptr obj = _fmodel->getObjectMeta()->getObject();
    RFeatures::ObjModelIntegrityChecker oic( obj);
    oic.checkIntegrity();   // Lengthy!

    ui->numGeometryVerticesLabel->setText( QString("%1").arg( obj->getNumVertices()));
    ui->numTextureVerticesLabel->setText( QString("%1").arg( obj->getNumVertices()));
    ui->numPolygonsLabel->setText( QString("%1").arg( obj->getNumFaces()));
    ui->numBoundaryLabel->setText( QString("%1").arg( oic.getNumEdge()));

    ui->numNonFlatLabel->setText( QString("%1").arg( oic.getNumNonFlat()));
    ui->numUnconnectedLabel->setText( QString("%1").arg( oic.getNumUnconnected()));
    ui->numFlatJunctionLabel->setText( QString("%1").arg( oic.getNumFlatJunction()));
    ui->numNonFlatJunctionALabel->setText( QString("%1").arg( oic.getNumNonFlatJunctionAType()));
    ui->numNonFlatJunctionBLabel->setText( QString("%1").arg( oic.getNumNonFlatJunctionBType()));
}   // end setModel


// public
void ModelInfoDialog::refresh() { setModel( _fmodel);}   // end refresh
