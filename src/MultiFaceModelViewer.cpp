#include <MultiFaceModelViewer.h>
#include <FeatureUtils.h>
#include <QImageTools.h>
#include <QHBoxLayout>
#include <QIcon>
using FaceApp::MultiFaceModelViewer;
using FaceTools::FaceModelViewer;
using FaceTools::FaceControl;
using FaceTools::FaceModel;

namespace
{

void addButton( QHBoxLayout* l, const char* iconres, QAction* a)
{
    a->setIcon( QIcon( iconres));
    QToolButton* b = new QToolButton;
    b->setDefaultAction(a);
    b->setMinimumSize( QSize(22,22));
    b->setMaximumSize( QSize(22,22));
    b->setIconSize( QSize(22,22));
    l->addWidget(b);
}   // end addButton

}   // end namespace


// private
void MultiFaceModelViewer::addCommonButtons( QHBoxLayout* l, FaceModelViewer* viewer)
{
    //addButton( l, ":/icons/lights.png", viewer->getLightsToggler());
    //addButton( l, ":/icons/reset_camera.png", viewer->getCameraResetter());
    addButton( l, ":/icons/screenshot.png", viewer->getScreenshotSaver());
}   // end addCommonButtons


// private
QToolButton* MultiFaceModelViewer::makeButton( QAction* action)
{
    QToolButton* b = new QToolButton;
    b->setDefaultAction(action);
    b->setMinimumSize( QSize(70,22));
    b->setMaximumSize( QSize(70,22));
    b->setIconSize( QSize(22,22));
    return b;
}   // makeButton


// public
MultiFaceModelViewer::MultiFaceModelViewer( const QList<QAction*>* actions, QMenu* cmenu, QWidget *parent)
    : QWidget(parent), _actions(actions)
{
    _v0 = new FaceModelViewer( cmenu, this); // Left
    _v1 = new FaceModelViewer( cmenu, this); // Middle
    _v2 = new FaceModelViewer( cmenu, this); // Right
    _activeViewer = _v0;

    QVBoxLayout* v0layout = new QVBoxLayout;
    v0layout->setContentsMargins(0,0,0,0);
    QVBoxLayout* v1layout = new QVBoxLayout;
    v1layout->setContentsMargins(0,0,0,0);
    QVBoxLayout* v2layout = new QVBoxLayout;
    v2layout->setContentsMargins(0,0,0,0);

    QIcon goLeftIcon( ":/icons/move_faces_left.png");
    QIcon goRightIcon( ":/icons/move_faces_right.png");
    QIcon copyLeftIcon( ":/icons/copy_faces_left.png");
    QIcon copyRightIcon( ":/icons/copy_faces_right.png");

    // Left panel actions
    _moveLeftToCentreAction = new QAction( goRightIcon, "Move Face(s) Right", this);
    connect( _moveLeftToCentreAction, &QAction::triggered, this, &MultiFaceModelViewer::moveLeftToCentre);
    _copyLeftToCentreAction = new QAction( copyRightIcon, "Copy Face(s) Right", this);
    connect( _copyLeftToCentreAction, &QAction::triggered, this, &MultiFaceModelViewer::copyLeftToCentre);

    // Right panel actions
    _moveRightToCentreAction = new QAction( goLeftIcon, "Move Face(s) Left", this);
    connect( _moveRightToCentreAction, &QAction::triggered, this, &MultiFaceModelViewer::moveRightToCentre);
    _copyRightToCentreAction = new QAction( copyLeftIcon, "Copy Face(s) Left", this);
    connect( _copyRightToCentreAction, &QAction::triggered, this, &MultiFaceModelViewer::copyRightToCentre);

    // Centre panel actions
    _moveCentreToRightAction = new QAction( goRightIcon, "Move Face(s) Right", this);
    connect( _moveCentreToRightAction, &QAction::triggered, this, &MultiFaceModelViewer::moveCentreToRight);
    _copyCentreToRightAction = new QAction( copyRightIcon, "Copy Face(s) Right", this);
    connect( _copyCentreToRightAction, &QAction::triggered, this, &MultiFaceModelViewer::copyCentreToRight);
    _moveCentreToLeftAction = new QAction( goLeftIcon, "Move Face(s) Left", this);
    connect( _moveCentreToLeftAction, &QAction::triggered, this, &MultiFaceModelViewer::moveCentreToLeft);
    _copyCentreToLeftAction = new QAction( copyLeftIcon, "Copy Face(s) Left", this);
    connect( _copyCentreToLeftAction, &QAction::triggered, this, &MultiFaceModelViewer::copyCentreToLeft);

    // Left panel
    v0layout->addWidget(_v0);

    QHBoxLayout* h0blayout = new QHBoxLayout;
    h0blayout->setContentsMargins(0,0,0,0);
    addCommonButtons( h0blayout, _v0);
    h0blayout->addStretch();
    h0blayout->addWidget( makeButton( _copyLeftToCentreAction));
    h0blayout->addWidget( makeButton( _moveLeftToCentreAction));
    v0layout->addLayout(h0blayout);

    // Right panel
    v2layout->addWidget(_v2);

    QHBoxLayout* h2blayout = new QHBoxLayout;
    h2blayout->setContentsMargins(0,0,0,0);
    h2blayout->addWidget( makeButton( _moveRightToCentreAction));
    h2blayout->addWidget( makeButton( _copyRightToCentreAction));
    h2blayout->addStretch();
    addCommonButtons( h2blayout, _v2);
    v2layout->addLayout(h2blayout);

    // Centre panel
    v1layout->addWidget(_v1);

    QHBoxLayout* h1blayout = new QHBoxLayout;
    h1blayout->setContentsMargins(0,0,0,0);
    h1blayout->addWidget( makeButton( _moveCentreToLeftAction));
    h1blayout->addWidget( makeButton( _copyCentreToLeftAction));
    h1blayout->addStretch();
    addCommonButtons( h1blayout, _v1);
    h1blayout->addStretch();
    h1blayout->addWidget( makeButton( _copyCentreToRightAction));
    h1blayout->addWidget( makeButton( _moveCentreToRightAction));
    v1layout->addLayout( h1blayout);

    QWidget* w0 = new QWidget;
    w0->setLayout(v0layout);
    QWidget* w1 = new QWidget;
    w1->setLayout(v1layout);
    QWidget* w2 = new QWidget;
    w2->setLayout(v2layout);

    _splitter = new QSplitter;
    _splitter->addWidget( w0);
    _splitter->addWidget( w1);
    _splitter->addWidget( w2);

    setLayout( new QVBoxLayout);
    layout()->addWidget( _splitter);

    connect( _v0, &FaceModelViewer::updatedSelected, this, &MultiFaceModelViewer::onLeftViewerUpdatedSelected);
    connect( _v1, &FaceModelViewer::updatedSelected, this, &MultiFaceModelViewer::onCentreViewerUpdatedSelected);
    connect( _v2, &FaceModelViewer::updatedSelected, this, &MultiFaceModelViewer::onRightViewerUpdatedSelected);

    _splitter->setCollapsible(1,false);
    setLeftViewerVisible(false);
    setRightViewerVisible(false);
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
}   // end ctor


// public
MultiFaceModelViewer::~MultiFaceModelViewer()
{
    _v0->disconnect(this);
    _v1->disconnect(this);
    _v2->disconnect(this);
    boost::unordered_set<FaceModel*> fmodels;
    foreach ( FaceControl* fcont, _fconts)
        fmodels.insert(fcont->getModel());
    foreach ( FaceModel* fmodel, fmodels)
        erase( fmodel);
}   // end dtor


// public
void MultiFaceModelViewer::erase( FaceModel* fmodel)
{
    FaceControl* f0 = _v0->take( fmodel);
    if ( f0)    // Left panel
    {
        _fconts.erase(f0);
        delete f0;
        setLeftViewerVisible( _v0->getNumModels() > 0);
    }   // end if
    FaceControl* f1 = _v1->take( fmodel);
    if ( f1)
    {
        _fconts.erase(f1);
        delete f1;
    }   // end if
    FaceControl* f2 = _v2->take( fmodel);
    if ( f2)
    {
        _fconts.erase(f2);
        delete f2;
        setRightViewerVisible( _v2->getNumModels() > 0);
    }   // end if
    _v0->updateRender();
    _v1->updateRender();
    _v2->updateRender();
}   // end erase


// public slot
void MultiFaceModelViewer::doOnOptionsChanged( const FaceTools::ModelOptions& opts)
{
    _v0->applyOptions(opts);
    _v1->applyOptions(opts);
    _v2->applyOptions(opts);
}   // end doOnOptionsChanged


// public slot
void MultiFaceModelViewer::setLeftViewerVisible(bool visible)
{
    QList<int> widths = _splitter->sizes();
    int sum = widths[0] + widths[1] + widths[2];
    if ( visible && widths[0] == 0)
        widths[0] = (int)(double(widths[1])/2);
    else if ( !visible && widths[0] > 0)
        widths[0] = 0;
    _splitter->setSizes( widths);
}   // end setLeftViewerVisible


// public slot
void MultiFaceModelViewer::setRightViewerVisible(bool visible)
{
    QList<int> widths = _splitter->sizes();
    int sum = widths[0] + widths[1] + widths[2];
    if ( visible && widths[2] == 0)
        widths[2] = (int)(double(widths[1])/2);
    else if ( !visible && widths[2] > 0)
        widths[2] = 0;
    _splitter->setSizes( widths);
}   // end setRightViewerVisible


// public slot
void MultiFaceModelViewer::insert( FaceModel* fmodel)
{
    FaceControl* fcont = new FaceControl(fmodel);
    foreach ( QAction* action, *_actions)
        fcont->addController(action);
    _fconts.insert( fcont);
    _v1->give( fcont);
}   // end insert


// public slot
void MultiFaceModelViewer::resetCamera()
{
    _activeViewer->getCameraResetter()->trigger();
}   // end resetCamera


// public slot
void MultiFaceModelViewer::saveScreenshot()
{
    std::vector<cv::Mat> mimgs;
    if ( _splitter->sizes()[0] > 0)
        mimgs.push_back( _v0->grabImage());
    if ( _splitter->sizes()[1] > 0)
        mimgs.push_back( _v1->grabImage());
    if ( _splitter->sizes()[2] > 0)
        mimgs.push_back( _v2->grabImage());
    cv::Mat m = RFeatures::concatHorizontalMax( mimgs);
    QTools::saveImage( m);
}   // end saveScreenshot


// private
void MultiFaceModelViewer::moveViews( FaceModelViewer* sv, FaceModelViewer* tv)
{
    boost::unordered_set<FaceModel*> fms;
    sv->getSelectedModels( &fms);
    foreach ( FaceModel* fmodel, fms)
    {
        FaceControl* fcont = sv->take(fmodel);
        if ( !tv->get( fmodel))
            tv->give( fcont);
        else
        {
            delete fcont;   // No longer needed on the source viewer
            tv->setControlled( fmodel, true, false);
            setActive( tv->get(fmodel), true);         // Inform clients
        }   // end else
        tv->updateRender();
        sv->updateRender();
    }   // end foreach
}   // end moveViews


// private
void MultiFaceModelViewer::copyViews( FaceModelViewer* sv, FaceModelViewer* tv)
{
    boost::unordered_set<FaceModel*> umodels;
    canCopyTo( sv, tv, umodels);
    foreach ( FaceModel* fmodel, umodels)
    {
        FaceControl* fcont = new FaceControl(fmodel);   // Create the new view
        fcont->setOptions( sv->get(fmodel)->getOptions());   // Copy across the options to the new view
        foreach ( QAction* action, *_actions)
            fcont->addController(action);
        _fconts.insert( fcont);
        tv->give( fcont);
    }   // end foreach
    tv->updateRender();
}   // end copyViews


// private slot
void MultiFaceModelViewer::moveLeftToCentre()
{
    moveViews( _v0, _v1);
    checkEnableLeftToCentre();
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
    setLeftViewerVisible( _v0->getNumModels() > 0);
}   // end moveLeftToCentre


// private slot
void MultiFaceModelViewer::copyLeftToCentre()
{
    copyViews( _v0, _v1);
    checkEnableLeftToCentre();
    deactivateAllSelectedOnViewer( _v0);
}   // end copyLeftToCentre


// private slot
void MultiFaceModelViewer::moveCentreToLeft()
{
    moveViews( _v1, _v0);
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
    checkEnableLeftToCentre();
    setLeftViewerVisible(true);
}   // end moveCentreToLeft


// private slot
void MultiFaceModelViewer::copyCentreToLeft()
{
    copyViews( _v1, _v0);
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
    setLeftViewerVisible(true);
    deactivateAllSelectedOnViewer( _v1);
}   // end copyCentreToLeft


// private slot
void MultiFaceModelViewer::moveCentreToRight()
{
    moveViews( _v1, _v2);
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
    checkEnableRightToCentre();
    setRightViewerVisible(true);
}   // end moveCentreToRight


// private slot
void MultiFaceModelViewer::copyCentreToRight()
{
    copyViews( _v1, _v2);
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
    setRightViewerVisible(true);
    deactivateAllSelectedOnViewer( _v1);
}   // end copyCentreToRight


// private slot
void MultiFaceModelViewer::moveRightToCentre()
{
    moveViews( _v2, _v1);
    checkEnableRightToCentre();
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
    setRightViewerVisible( _v2->getNumModels() > 0);
}   // moveRightToCentre


// private slot
void MultiFaceModelViewer::copyRightToCentre()
{
    copyViews( _v2, _v1);
    checkEnableRightToCentre();
    deactivateAllSelectedOnViewer( _v2);
}   // copyRightToCentre


// private
void MultiFaceModelViewer::deactivateAllSelectedOnViewer( FaceModelViewer* viewer)
{
    boost::unordered_set<FaceModel*> fms;
    viewer->getSelectedModels( &fms);
    foreach ( FaceModel* fm, fms)
    {
        viewer->setControlled( fm, false, false);   // Don't allow FaceModelViewer::updateSelected to be emitted again!
        setActive( viewer->get(fm), false);         // Inform clients
    }   // end foreach
}   // end deactivateAllSelectedOnViewer


// private slot
void MultiFaceModelViewer::onLeftViewerUpdatedSelected( FaceControl* fcont, bool v)
{
    checkEnableLeftToCentre();
    if ( v) // Ensure that models in the other viewers are not controlled.
    {
        deactivateAllSelectedOnViewer( _v1);
        deactivateAllSelectedOnViewer( _v2);
    }   // end if
    _activeViewer = _v0;
    emit setActive( fcont, v);
}   // end onLeftViewerUpdatedSelected


// private slot
void MultiFaceModelViewer::onCentreViewerUpdatedSelected( FaceControl* fcont, bool v)
{
    checkEnableCentreToLeft();
    checkEnableCentreToRight();
    if ( v) // Ensure that models in the other viewers are not controlled.
    {
        deactivateAllSelectedOnViewer( _v0);
        deactivateAllSelectedOnViewer( _v2);
    }   // end if
    checkEnableLeftToCentre();
    checkEnableRightToCentre();
    _activeViewer = _v1;
    emit setActive( fcont, v);
}   // end onCentreViewerUpdatedSelected


// private slot
void MultiFaceModelViewer::onRightViewerUpdatedSelected( FaceControl* fcont, bool v)
{
    checkEnableRightToCentre();
    if ( v) // Ensure that models in the other viewers are not controlled.
    {
        deactivateAllSelectedOnViewer( _v0);
        deactivateAllSelectedOnViewer( _v1);
    }   // end if
    _activeViewer = _v2;
    emit setActive( fcont, v);
}   // end onRightViewerUpdatedSelected


// private
void MultiFaceModelViewer::checkEnableLeftToCentre()
{
    _moveLeftToCentreAction->setEnabled( _v0->getSelectedModels() > 0);   // Move button
    boost::unordered_set<FaceModel*> tov1;
    canCopyTo( _v0, _v1, tov1);
    _copyLeftToCentreAction->setEnabled( !tov1.empty());   // Copy button
}   // end checkEnableLeftToCentre


// private
void MultiFaceModelViewer::checkEnableCentreToLeft()
{
    _moveCentreToLeftAction->setEnabled( _v1->getSelectedModels() > 0);   // Move button
    boost::unordered_set<FaceModel*> tov0;
    const bool goleft = canCopyTo( _v1, _v0, tov0) > 0;
    _copyCentreToLeftAction->setEnabled( goleft);   // Copy button
}   // end checkEnableCentreToLeft


// private
void MultiFaceModelViewer::checkEnableCentreToRight()
{
    _moveCentreToRightAction->setEnabled( _v1->getSelectedModels() > 0);   // Move button
    boost::unordered_set<FaceModel*> tov2;
    const bool goright = canCopyTo( _v1, _v2, tov2) > 0;
    _copyCentreToRightAction->setEnabled( goright);   // Copy button
}   // end checkEnableCentreToRight


// private
void MultiFaceModelViewer::checkEnableRightToCentre()
{
    _moveRightToCentreAction->setEnabled( _v2->getSelectedModels() > 0);   // Move button
    boost::unordered_set<FaceModel*> tov1;
    canCopyTo( _v2, _v1, tov1);
    _copyRightToCentreAction->setEnabled( !tov1.empty());   // Copy button
}   // end checkEnableRightToCentre


// private
// Find the set of models than can go (move or copy) to tv from sv
size_t MultiFaceModelViewer::canCopyTo( FaceModelViewer* sv, FaceModelViewer* tv, boost::unordered_set<FaceModel*>& totv) const
{
    boost::unordered_set<FaceModel*> fms;
    sv->getSelectedModels( &fms);
    totv = fms; // Copy
    foreach ( FaceModel* fm, fms)
        if ( tv->get(fm))
            totv.erase(fm);
    return totv.size();
}   // end canCopyTo

