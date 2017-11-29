#include <FaceAppMain.h>
#include <ui_FaceAppMain.h>
#include <SplashWindow.h>
#include <ActionExportPDF.h>

#include <CoreVisualisations.h>
#include <PluginsLoader.h>      // QTools
#include <VisualisationAction.h>
#include <ViewActions.h>    // static plugin
#include <ToolActions.h>    // static plugin
#include <QLabel>
#include <QMimeData>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <FaceModel.h>          // FaceTools
using FaceApp::FaceAppMain;
using FaceApp::FaceModelManager;

#include <U3DExporter.h>
#include <PDFGenerator.h>


// private
void FaceAppMain::createFileMenu()
{
    ui->menu_File->addAction( _fmm.getLoadAction());
    ui->menu_File->addAction( _fmm.getSaveAction());
    ui->menu_File->addAction( _fmm.getSaveAsAction());
    ui->menu_File->addAction( _fmm.getCloseAction());
    ui->menu_File->addSeparator();

    RModelIO::PDFGenerator::pdflatex = PDF_LATEX;
    RModelIO::U3DExporter::IDTFConverter = IDTF_CONVERTER;
    FaceApp::ActionExportPDF* expdf = new FaceApp::ActionExportPDF( "Export PDF", ":/icons/pdf.png", _progressBar);
    expdf->setWorkingDir( LATEX_WORK);
    expdf->setLogo( LATEX_LOGO);
    _allActions << expdf->qaction();
    ui->menu_File->addAction( expdf->qaction());

    connect( ui->actionModel_Info, &QAction::triggered, _dialogs->modelInfoDialog(), &QDialog::show);
    connect( ui->actionModel_Info, &QAction::triggered, _dialogs->modelInfoDialog(), &QDialog::raise);
    ui->menu_File->addAction( ui->actionModel_Info);
    ui->menu_File->addSeparator();
    ui->menu_File->addAction( ui->action_Exit);
}   // end createFileMenu


// private
void FaceAppMain::createHelpMenu()
{
    // Help
    connect( ui->actionHelp, &QAction::triggered, _dialogs->helpDialog(), &QDialog::show);
    connect( ui->actionHelp, &QAction::triggered, _dialogs->helpDialog(), &QDialog::raise);
    ui->menu_Help->addAction( ui->actionHelp);

    // About
    connect( ui->actionAbout_3D_FAST, &QAction::triggered, _dialogs->aboutDialog(), &QDialog::show);
    connect( ui->actionAbout_3D_FAST, &QAction::triggered, _dialogs->aboutDialog(), &QDialog::raise);
    ui->menu_Help->addAction( ui->actionAbout_3D_FAST);

    // Licenses
    connect( ui->actionLicensing, &QAction::triggered, _dialogs->licensesDialog(), &QDialog::show);
    connect( ui->actionLicensing, &QAction::triggered, _dialogs->licensesDialog(), &QDialog::raise);
    ui->menu_Help->addAction( ui->actionLicensing);

    ui->menu_Help->addSeparator();

    // Plugins
    connect( ui->actionAbout_Plugins, &QAction::triggered, _dialogs->pluginsDialog(), &QDialog::show);
    connect( ui->actionAbout_Plugins, &QAction::triggered, _dialogs->pluginsDialog(), &QDialog::raise);
    ui->menu_Help->addAction( ui->actionAbout_Plugins);
}   // end createHelpMenu


// private
void FaceAppMain::finishMenus()
{
    // View menu
    ui->menu_View->addAction( ui->actionReset_Camera);
    ui->menu_View->addAction( ui->actionSave_Screenshot);
    ui->menu_View->addSeparator();
    ui->menu_View->addAction( ui->action_ShowVisToolbar);

    // Tools menu
    ui->menu_Tools->addSeparator();

    // Landmarks
    connect( ui->action_Landmarks, &QAction::triggered, _dialogs->landmarksDialog(), &QDialog::show);
    connect( ui->action_Landmarks, &QAction::triggered, _dialogs->landmarksDialog(), &QDialog::raise);
    ui->menu_Tools->addAction( ui->action_Landmarks);

    // Options
    connect( ui->action_Options, &QAction::triggered, _dialogs->optionsDialog(), &QDialog::show);
    connect( ui->action_Options, &QAction::triggered, _dialogs->optionsDialog(), &QDialog::raise);
    ui->menu_Tools->addAction( ui->action_Options);
}   // end finishMenus


// private
void FaceAppMain::finishMainToolbar()
{
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction( ui->action_Options);

    QWidget* emptySpacer = new QWidget();
    emptySpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->mainToolBar->addWidget(emptySpacer);

    QLabel* label = new QLabel();
    label->setPixmap(QPixmap( ":/logos/logo.png"));
    ui->mainToolBar->addWidget(label);
}   // end finishMainToolbar


// private
void FaceAppMain::setAction( FaceTools::FaceAction* faction)
{
    if ( qobject_cast<FaceTools::FileIOInterface*>(faction))
        _fmm.addFileFormat( qobject_cast<FaceTools::FileIOInterface*>(faction));
    else
    {
        _allActions << faction->qaction();
        if ( qobject_cast<FaceTools::VisualisationAction*>(faction))
            ui->visToolBar->addAction( faction->qaction());
    }   // end else
}   // end setAction


// private
void FaceAppMain::loadPlugins()
{
    const QString dllsDir = QApplication::applicationDirPath() + "/plugins";
    QTools::PluginsLoader ploader( dllsDir.toStdString());
    std::cerr << "DLLs directory: " << ploader.getPluginsDir().absolutePath().toStdString() << std::endl;
    connect( &ploader, &QTools::PluginsLoader::loadedPlugin, this, &FaceAppMain::setPlugin);
    ploader.loadPlugins();
    _dialogs->pluginsDialog()->addPlugins( ploader);
}   // end loadPlugins


// private slot
void FaceAppMain::setPlugin( QTools::PluginInterface* plugin)
{
    // Set View actions
    FaceApp::ViewActions* vactions = qobject_cast<FaceApp::ViewActions*>(plugin);
    if ( vactions)
    {
        foreach ( const QString& iid, vactions->getInterfaceIds())
            _allActions << vactions->getInterface(iid)->qaction();
        ui->mainToolBar->addSeparator();
        vactions->addTo( ui->mainToolBar);
        vactions->addTo( ui->menu_View);
        vactions->addToContext( &_contextMenu);
        return;
    }   // end if

    // Set Tool actions
    FaceApp::ToolActions* tactions = qobject_cast<FaceApp::ToolActions*>(plugin);
    if ( tactions)
    {
        tactions->initFaceDetection( HAAR_CASCADES_MODELS, FACE_SHAPE_LANDMARKS);
        foreach ( const QString& iid, tactions->getInterfaceIds())
            _allActions << tactions->getInterface(iid)->qaction();
        ui->mainToolBar->addSeparator();
        tactions->addTo( ui->mainToolBar);  // Add tools to the main tool bar
        tactions->addTo( ui->menu_Tools);
        tactions->addToContext( &_contextMenu);
        return;
    }   // end if

    // Configure core visualisations set
    FaceApp::CoreVisualisations* vis = qobject_cast<FaceApp::CoreVisualisations*>(plugin);
    if ( vis)
    {
        vis->addTo( ui->visToolBar);
        foreach ( const QString& iid, vis->getInterfaceIds())
            _allActions << vis->getInterface(iid)->qaction();
        return;
    }   // end if

    // Some other FaceActionGroup?
    FaceTools::FaceActionGroup* fag = qobject_cast<FaceTools::FaceActionGroup*>(plugin);
    if ( fag)
    {
        foreach ( const QString& iid, fag->getInterfaceIds())
            setAction( fag->getInterface(iid));
        return;
    }   // end if

    // Some other action?
    FaceTools::FaceAction* faction = qobject_cast<FaceTools::FaceAction*>(plugin);
    if ( faction)
    {
        setAction( faction);
        return;
    }   // end if
}   // end setPlugin


// public
FaceAppMain::FaceAppMain()
    : QMainWindow(NULL), ui(new Ui::FaceAppMain),
      _dialogs(NULL), _mviewer(NULL), _progressBar(NULL)
{
    ui->setupUi(this);
    setAcceptDrops(true);   // Accept dropping of files onto this widget

    ui->mainToolBar->addAction( _fmm.getLoadAction());
    ui->mainToolBar->addAction( _fmm.getSaveAction());
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction( ui->actionSave_Screenshot);
    ui->mainToolBar->addAction( ui->actionReset_Camera);

    _dialogs = new FaceApp::CoreDialogs;
    loadPlugins();

    createFileMenu();
    createHelpMenu();
    finishMenus();
    finishMainToolbar();

    _mviewer = new MultiFaceModelViewer( &_allActions, &_contextMenu, this);
    QWidget* cwidget = new QWidget;
    cwidget->setLayout( new QVBoxLayout);
    cwidget->layout()->setContentsMargins(0,0,0,0);
    cwidget->layout()->addWidget( _mviewer);
    _progressBar = new QProgressBar;
    _progressBar->setTextVisible(false);
    _progressBar->setMinimum(0);
    _progressBar->setMaximum(100);
    _progressBar->setMaximumHeight(10);
    cwidget->layout()->addWidget( _progressBar);

    setCentralWidget( cwidget);

    _dialogs->loadOptions( APP_OPTION_PREFS);

    // Locate centrally on desktop
    setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, sizeHint(), qApp->desktop()->availableGeometry()));

    connect( _dialogs, &FaceApp::CoreDialogs::onOptionsChanged, _mviewer, &MultiFaceModelViewer::doOnOptionsChanged);
    connect( &_fmm, &FaceModelManager::startedClose, _mviewer, &MultiFaceModelViewer::erase);
    connect( &_fmm, &FaceModelManager::finishedSave, this, &FaceAppMain::finishedSave);
    connect( &_fmm, &FaceModelManager::finishedLoad, this, &FaceAppMain::finishedLoad);
    connect( _mviewer, &MultiFaceModelViewer::setActive, this, &FaceAppMain::setActive);

    connect( ui->actionReset_Camera, &QAction::triggered, _mviewer, &FaceApp::MultiFaceModelViewer::resetCamera);
    connect( ui->actionSave_Screenshot, &QAction::triggered, _mviewer, &FaceApp::MultiFaceModelViewer::saveScreenshot);

    setActive(NULL, false);
    FaceApp::SplashWindow *splash = new FaceApp::SplashWindow;
    connect( splash, &FaceApp::SplashWindow::clickedLink, ui->actionLicensing, &QAction::triggered);
    splash->show();
}   // end ctor


// public
FaceAppMain::~FaceAppMain()
{
    delete _dialogs;
    delete ui;
}   // end dtor


// public slot
bool FaceAppMain::loadModel( const QString& fname)
{
    const bool success = _fmm.load( fname.toStdString());
    if ( !success)
        std::cerr << _fmm.getError() << std::endl;
    return success;
}   // end loadModel



// protected virtual
void FaceAppMain::dragEnterEvent( QDragEnterEvent *evt)
{
    if ( evt->mimeData()->hasText())
    {
        QString fname = evt->mimeData()->text();
        if ( fname.startsWith("file:///"))
        {
            fname = fname.remove(0, QString("file:///").size()).trimmed();
            if ( _fmm.canLoad( fname))
                evt->acceptProposedAction();
        }   // end if
    }   // end if
}   // end dragEnterEvent


// protected virtual
void FaceAppMain::dropEvent( QDropEvent *evt)
{
    QString fname = evt->mimeData()->text();
    fname = fname.remove(0, QString("file:///").size()).trimmed();
    if ( loadModel(fname))
        evt->acceptProposedAction();
}   // end dropEvent


// protected virtual
void FaceAppMain::closeEvent( QCloseEvent* evt)
{
    bool doclose = true;
    // Prevent application closing if the user has changes they still want to save.
    if ( _fmm.getNumUnsaved() > 0)
    {
        QMessageBox::StandardButton reply = QMessageBox::question( this, "Exit?", "Unsaved changes will be lost! Really exit?",
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        doclose = reply == QMessageBox::Yes;
    }   // end if

    if ( doclose)
    {
        _dialogs->hideAll();   // Ensure all GUI windows closed
        _dialogs->setActive(NULL);
        _dialogs->saveOptions( APP_OPTION_PREFS);
        _fmm.closeAllModels();
    }   // end if
    else
        evt->ignore();
}   // end closeEvent


// protected virtual
QSize FaceAppMain::sizeHint() const { return QSize( 1000, 600);}   // end sizeHint


// private slot
void FaceAppMain::finishedLoad( FaceTools::FaceModel* fmodel)
{
    if ( fmodel)
    {
        _mviewer->insert(fmodel);
        _mviewer->doOnOptionsChanged( _dialogs->getOptions());
    }   // end if
    else
        QMessageBox::warning( this, tr("Load Error!"), _fmm.getError().c_str());
}   // end finishedLoad


// private slot
void FaceAppMain::finishedSave( FaceTools::FaceModel* fmodel)
{
    if ( fmodel)
    {
        const std::string savefilename = fmodel->getObjectMeta()->getObjectFile();
        std::cerr << "Saved model to " << savefilename << std::endl;
    }   // end if
    else
        QMessageBox::warning( this, tr("Save Error!"), _fmm.getError().c_str());
}   // end finishedSave


// private slot
void FaceAppMain::setActive( FaceTools::FaceControl* fcont, bool v)
{
    if ( fcont)
        _fmm.setActive( fcont->getModel(), v);
    _dialogs->setActive( v ? fcont : NULL);
    ui->actionModel_Info->setEnabled(v);
    ui->action_Landmarks->setEnabled(v);
}   // end setActive
