#include <LandmarksDialog.h>
#include <ui_LandmarksDialog.h>
#include <QStandardItemModel>
#include <QAbstractButton>
#include <sstream>
#include <iomanip>
#include <iostream>
using FaceApp::LandmarksDialog;
using FaceTools::Landmarks::Landmark;
using FaceTools::FaceControl;
using FaceTools::ObjMetaData;


enum ColumnIndices
{
    MARKER_HIDDEN_ID = 0,
    MARKER_NAME = 1,
    MARKER_POSITION = 2,
    MARKER_SHOW = 3,
    MARKER_MOVABLE = 4,
    MARKER_DELETABLE = 5
};  // end enum


LandmarksDialog::LandmarksDialog( QWidget *parent) :
    QDialog(parent), ui(new Ui::LandmarksDialog), _fcont(NULL)
{
    ui->setupUi(this);
    setWindowFlags( Qt::WindowCloseButtonHint);
    setWindowIcon(QIcon(":/icons_gui/3dfast_128x128.png"));

    // Will display five landmark fields
    QStandardItemModel *model = new QStandardItemModel(0, 6, this);
    model->setHorizontalHeaderItem( MARKER_HIDDEN_ID, new QStandardItem(QString("ID")));
    model->setHorizontalHeaderItem( MARKER_NAME, new QStandardItem(QString("Name")));
    model->setHorizontalHeaderItem( MARKER_POSITION, new QStandardItem(QString("Position (X,Y,Z)")));
    model->setHorizontalHeaderItem( MARKER_SHOW, new QStandardItem(QString("Show?")));
    model->setHorizontalHeaderItem( MARKER_MOVABLE, new QStandardItem(QString("Positionable?")));
    model->setHorizontalHeaderItem( MARKER_DELETABLE, new QStandardItem(QString("Deletable?")));
    ui->landmarksTableView->setModel(model);
    ui->landmarksTableView->setColumnHidden(0, true);   // Hide the MARKER_HIDDEN_ID column
    ui->landmarksTableView->setSelectionMode( QAbstractItemView::ExtendedSelection);    // Ctrl to select multiple items

    QHeaderView* hheader = ui->landmarksTableView->horizontalHeader();
    hheader->setStretchLastSection(true);
    ui->landmarksTableView->resizeColumnsToContents();

    connect( model, &QStandardItemModel::itemChanged, this, &LandmarksDialog::doOnItemChanged);
}   // end ctor


LandmarksDialog::~LandmarksDialog()
{
    delete ui;
}   // end dtor


// private
void LandmarksDialog::clear()
{
    QAbstractItemModel* amodel = ui->landmarksTableView->model();
    amodel->removeRows(0, amodel->rowCount());
    _markerRows.clear();
}   // end clear


// private
void LandmarksDialog::connectModel()
{
    connect( _fcont, &FaceControl::onEnteringLandmark, this, &LandmarksDialog::doOnLandmarkHighlighted);
    connect( _fcont, &FaceControl::onExitingLandmark, this, &LandmarksDialog::doOnLandmarkHighlighted);
    connect( _fcont, &FaceControl::metaUpdated, this, &LandmarksDialog::resetLandmarks);
}   // end connectModel


// protected
void LandmarksDialog::showEvent( QShowEvent*)
{
    if ( _fcont)    // Ensure landmarks are shown
        _fcont->showLandmarks(true);
}   // end showEvent


// public
void LandmarksDialog::setModel( FaceControl* fcont)
{
    if ( _fcont)
        _fcont->disconnect(this);
    _fcont = fcont;
    if ( _fcont == NULL)
        return;
    connectModel();
    resetLandmarks();
}   // end setView


// private slot
void LandmarksDialog::resetLandmarks()
{
    assert( _fcont);
    clear();
    // Add the landmarks in sorted order
    boost::unordered_set<std::string> lmset;
    _fcont->getModel()->getObjectMeta()->getLandmarks( lmset);
    std::vector<std::string> lmnames( lmset.begin(), lmset.end());
    std::sort( lmnames.begin(), lmnames.end());
    foreach ( const std::string& lm, lmnames)
        appendRow( lm);
    ui->landmarksTableView->resizeColumnsToContents();
}   // end resetLandmarks


// private
void LandmarksDialog::removeRow( const std::string& lm)
{
    QStandardItemModel* model = (QStandardItemModel*)ui->landmarksTableView->model();
    model->removeRow( _markerRows.at(lm));
    _markerRows.erase( lm);
}   // end removeRow


// private
void LandmarksDialog::appendRow( const std::string& lm)
{
    assert( _markerRows.count(lm) == 0);
    QList<QStandardItem*> rowItems;
    createRow( lm, rowItems);
    QStandardItemModel* model = (QStandardItemModel*)ui->landmarksTableView->model();
    _markerRows[lm] = model->rowCount();  // Set the lookup row index for this marker
    model->appendRow( rowItems);
}   // end appendRow


namespace
{

std::string getPositionString( const cv::Vec3f& pos)
{
    std::ostringstream oss;
    oss << std::fixed << std::right << std::setprecision(2) << std::setw(16)
                      << pos[0] << ", " << pos[1] << ", " << pos[2];
    return oss.str();
}   // end getPositionString

}   // end namespace


// private
void LandmarksDialog::createRow( const std::string& mname, QList<QStandardItem*>& rowItems)
{
    assert( _fcont);
    const ObjMetaData::Ptr objmeta = _fcont->getModel()->getObjectMeta();
    assert( objmeta->hasLandmark(mname));
    const Landmark* lm = objmeta->getLandmarkMeta(mname);

    // MARKER_HIDDEN_ID
    rowItems << new QStandardItem( mname.c_str());

    // MARKER_NAME (editable)
    QStandardItem* nameItem = new QStandardItem(mname.c_str());
    nameItem->setEditable(true);
    rowItems << nameItem;

    // MARKER_POSITION
    rowItems << new QStandardItem( getPositionString( lm->pos).c_str());

    // MARKER_SHOW
    QStandardItem *showCheckBox = new QStandardItem;
    showCheckBox->setCheckable(true);
    showCheckBox->setCheckState( lm->visible ? Qt::Checked : Qt::Unchecked);
    rowItems << showCheckBox;

    // MARKER_MOVABLE
    QStandardItem *posCheckBox = new QStandardItem;
    posCheckBox->setCheckable(true);
    posCheckBox->setCheckState( lm->movable ? Qt::Checked : Qt::Unchecked);
    rowItems << posCheckBox;

    // MARKER_DELETABLE
    QStandardItem *delCheckBox = new QStandardItem;
    delCheckBox->setCheckable(true);
    delCheckBox->setCheckState( lm->deletable ? Qt::Checked : Qt::Unchecked);
    rowItems << delCheckBox;

    /*
    QStandardItem *orientCheckBox = new QStandardItem;
    orientCheckBox->setCheckable(true);
    orientCheckBox->setCheckState( Qt::Checked);
    orientCheckBox->setEnabled(false);  // Don't allow user to check!
    rowItems << delCheckBox;
    */
}   // end createRow


// private
QStandardItem* LandmarksDialog::getRowItem( const std::string& lm, int colIdx)
{
    assert( _markerRows.count(lm) > 0);
    const int rowi = _markerRows.at(lm);
    QStandardItemModel* model = (QStandardItemModel*)ui->landmarksTableView->model();
    return model->item(rowi, colIdx);
}   // end getRowItem


// private
std::string LandmarksDialog::getLandmarkFromRowId( int rowIdx) const
{
    QStandardItemModel* model = (QStandardItemModel*)ui->landmarksTableView->model();
    QStandardItem* iditem = model->item( rowIdx, MARKER_HIDDEN_ID);
    return iditem->text().toStdString();
}   // end getLandmarkFromRowId


// private slot
void LandmarksDialog::doOnLandmarkHighlighted( std::string lm, const QPoint&)
{
    assert(_fcont);
    int rowid = -1;
    if ( _markerRows.count(lm) > 0)
        rowid = _markerRows.at(lm);
    if ( _fcont->isLandmarkPointedAt() != lm) // Slot might be due to cursor moving off landmark
        rowid = -1;
    ui->landmarksTableView->selectRow( rowid);
}   // end doOnLandmarkHighlighted


// private slot
void LandmarksDialog::doOnItemChanged( QStandardItem* m)
{
    if ( !isVisible() || !isEnabled())
        return;

    if ( m->row() < 0)
        return;

    // Don't want signals emitted from the FaceModel to be caught by this dialog...
    _fcont->disconnect(this);
    const std::string lm = getLandmarkFromRowId( m->row());
    switch ( m->column())
    {
        case MARKER_NAME:
            changeLandmarkName( lm, m->text().toStdString());
            break;
        case MARKER_SHOW:
            setMarkerVisible( lm, m->checkState() == Qt::Checked);
            break;
        case MARKER_MOVABLE:
            setMarkerMovable( lm, m->checkState() == Qt::Checked);
            break;
        case MARKER_DELETABLE:
            setMarkerDeletable( lm, m->checkState() == Qt::Checked);
            break;
    }   // end switch
    connectModel(); // ... until after changing the required values.
}   // end doOnItemChanged


// private
void LandmarksDialog::changeLandmarkName( const std::string& n0, const std::string& n1)
{
    assert(_fcont);
    ObjMetaData::Ptr objmeta = _fcont->getModel()->getObjectMeta();
    if ( objmeta->changeLandmarkName( n0, n1))
    {
        // Set the MARKER_HIDDEN_ID element to the new value
        QStandardItem* hiddenID = getRowItem( n0, MARKER_HIDDEN_ID);
        hiddenID->setText( n1.c_str());
        // Update the mapping key for the row index into the table
        _markerRows[n1] = _markerRows[n0];
        _markerRows.erase(n0);
    }   // end if
}   // end changeLandmarkName


// private
void LandmarksDialog::setMarkerVisible( const std::string& lm, bool visible)
{
    QStandardItem* checkBox = getRowItem( lm, MARKER_SHOW);
    checkBox->setCheckState( visible ? Qt::Checked : Qt::Unchecked);
    Landmark nlmk = *_fcont->getModel()->getObjectMeta()->getLandmarkMeta(lm);
    nlmk.visible = visible;
    _fcont->updateLandmark( nlmk);
}   // end setMarkerVisible


// private
void LandmarksDialog::setMarkerMovable( const std::string& lm, bool movable)
{
    QStandardItem* checkBox = getRowItem( lm, MARKER_MOVABLE);
    checkBox->setCheckState( movable ? Qt::Checked : Qt::Unchecked);
    Landmark nlmk = *_fcont->getModel()->getObjectMeta()->getLandmarkMeta(lm);
    nlmk.movable = movable;
    _fcont->updateLandmark( nlmk);
}   // end setMarkerMovable


// private
void LandmarksDialog::setMarkerDeletable( const std::string& lm, bool deletable)
{
    QStandardItem* checkBox = getRowItem( lm, MARKER_DELETABLE);
    checkBox->setCheckState( deletable ? Qt::Checked : Qt::Unchecked);
    Landmark nlmk = *_fcont->getModel()->getObjectMeta()->getLandmarkMeta(lm);
    nlmk.deletable = deletable;
    _fcont->updateLandmark( nlmk);
}   // end setMarkerDeletable

