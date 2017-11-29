#include <FaceModelManager.h>
#include <MiscFunctions.h>
#include <FaceTools.h>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <map>
using FaceApp::FaceModelManager;
using FaceTools::FileIOInterface;
using FaceTools::FaceModel;


// public
FaceModelManager::FaceModelManager()
    : _err(""),
    _loadAction( QIcon(":/icons/load.png"), "&Open", NULL),
    _saveAction( QIcon(":/icons/save.png"), "&Save", NULL),
    _saveAsAction( QIcon(":/icons/save_as.png"), "Save &As", NULL),
    _closeAction( QIcon(":/icons/save_close.png"), "&Close", NULL)
{
    connect( &_loadAction,   &QAction::triggered, this, &FaceModelManager::doLoad);
    connect( &_saveAction,   &QAction::triggered, this, &FaceModelManager::doSave);
    connect( &_saveAsAction, &QAction::triggered, this, &FaceModelManager::doSaveAs);
    connect( &_closeAction,  &QAction::triggered, this, &FaceModelManager::doClose);

    _loadAction.setShortcut( Qt::CTRL + Qt::Key_O);
    _saveAction.setShortcut( Qt::CTRL + Qt::Key_S);
    _closeAction.setShortcut( Qt::CTRL + Qt::Key_W);
    checkEnabledActions();
}  // end ctor


// public
FaceModelManager::~FaceModelManager()
{}  // end dtor


// public
void FaceModelManager::addFileFormat( FileIOInterface* fii)
{
    _iohandler.addFileFormat(fii);
}   // end addFileFormat


// public
bool FaceModelManager::canLoad( const QString& txt) const
{
    std::string fname = txt.toStdString();
    return boost::filesystem::exists( fname) &&
           _iohandler.getLoadInterface( fname) != NULL;
}   // end canLoad


// public
bool FaceModelManager::load( const std::string& fname)
{
    assert( !fname.empty());
    if ( fname.empty())
        return false;

    _err = "";
    std::cerr << "FaceModelManager::load: trying to load '" << fname << "'..." << std::endl;

    // If a model having the same file path is already loaded, just return.
    if ( _openModels.count(fname) > 0)
    {
        emit finishedLoad(_openModels.at(fname));
        return true;
    }   // end if

    FileIOInterface* fileio = _iohandler.getLoadInterface(fname);
    bool success = false;
    if ( !boost::filesystem::exists( fname))
        _err = "File '" + fname + "' does not exist!";
    else if ( !fileio)
        _err = "File '" + fname + "' has invalid file type!";
    else
    {
        success = true;
        fileio->setFileOp( fname, NULL);
        connect( fileio, &FileIOInterface::finishedImport, this, &FaceModelManager::postProcessLoad);
        fileio->process();  // Loads asynchronously
    }   // end else
    return success;
}   // end load


// public
bool FaceModelManager::save( FaceModel* fmodel, const std::string& savefilepath)
{
    assert( !savefilepath.empty());
    if ( savefilepath.empty())
        return false;

#ifndef NDEBUG
    const std::string &fname = fmodel->getFilePath();
    assert( _unsaved.count(fname) > 0);
#endif

    _err = "";
    FileIOInterface* fileio = _iohandler.getSaveInterface( savefilepath);
    bool success = false;
    if ( !fileio)
        _err = "File '" + savefilepath + "' has invalid file type!";
    else
    {
        success = true;
        fileio->setFileOp( savefilepath, fmodel);
        connect( fileio, &FileIOInterface::finishedExport, this, &FaceModelManager::postProcessSave);
        fileio->process();  // Saves asynchronously
    }   // end else
    return success;
}   // end save


// public
void FaceModelManager::closeModel( FaceModel* fmodel)
{
    assert(fmodel);
    emit startedClose( fmodel);
    const std::string &fname = fmodel->getFilePath();
    assert( _openModels.count(fname) > 0);
    if ( _unsaved.count(fname) > 0)
        std::cerr << "[INFO] FaceApp::FaceModelManager::closeModel: closing unsaved model!" << std::endl;
    _openModels.erase(fname);
    _unsaved.erase(fname);
    _active.erase(fname);
    delete fmodel;
    checkEnabledActions();
}   // end closeModel


// public
void FaceModelManager::closeAllModels()
{
    _unsaved.clear();
    while ( !_openModels.empty())
        closeModel( _openModels.begin()->second);
}   // end closeAllModels


// private slot
void FaceModelManager::postProcessSave()
{
    FileIOInterface* fileio = qobject_cast<FileIOInterface*>(sender());
    FaceModel* fmodel = fileio->getModel();
    fileio->disconnect(this);
    _err = fileio->getError();
    std::string fname = fileio->getFilePath();
    if ( _err.empty())
    {
        const std::string oldfname = fmodel->getFilePath();
        fmodel->setFilePath(fname);

        assert( _openModels.count( oldfname) > 0);
        _openModels.erase(oldfname);
        assert( _openModels.count( fname) == 0);
        _openModels[fname] = fmodel;
        _unsaved.erase( fname); // Erase this since above call to setFilePath causes onChanged to fire on the model (which updates _unsaved)
        _unsaved.erase( oldfname);
        if ( _active.count( oldfname) > 0)
        {
            _active.erase(oldfname);
            _active.insert(fname);
        }   // end if
        checkEnabledActions();
    }   // end if
    emit finishedSave( _err.empty() ? fmodel : NULL);
}   // end postProcessSave


// private slot
void FaceModelManager::postProcessLoad()
{
    FileIOInterface* fileio = qobject_cast<FileIOInterface*>(sender());
    fileio->disconnect(this);
    FaceModel* fmodel = fileio->getModel();
    if ( !fmodel)
    {
        _err = fileio->getError();
        emit finishedLoad( NULL);
        return;
    }   // end if

    std::string fname = fileio->getFilePath();
    FaceTools::ObjMetaData::Ptr omd = fmodel->getObjectMeta();
    RFeatures::ObjModel::Ptr model = omd->getObject();

    // Don't want models to have more than 1 texture map
    if ( model->getNumMaterials() > 1)
    {
        std::cerr << " =====[ Combining Textures ]=====" << std::endl;
        model->mergeMaterials();
    }   // end if

    // Clean the model - make a triangulated manifold.
    RFeatures::ObjModelIntegrityChecker ic( model);
    RFeatures::ObjModelIntegrityError ierror = ic.checkIntegrity();
    if ( ierror != RFeatures::NO_INTEGRITY_ERROR)
    {
        std::ostringstream oss;
        oss << "[ERROR] FaceApp::FaceModelManager::postProcessLoad: integrity error on load (CODE " << int(ierror) << ")";
        _err = oss.str();
        delete fmodel;
        emit finishedLoad( NULL);
        return;
    }
    else if ( !ic.is2DManifold())
    {
        std::cerr << " =====[ Cleaning Model ]=====" << std::endl;
        FaceTools::clean(model);
        ic.checkIntegrity();
        if ( !ic.is2DManifold())
        {
            delete fmodel;
            _err = "[ERROR] FaceApp::FaceModelManager::postProcessLoad: failed to make triangulated manifold from model!";
            emit finishedLoad( NULL);
            return;
        }   // end if
    }   // end if

    fmodel->setFilePath( fname); // Ensure that the filepath set correctly.
    _openModels[fname] = fmodel;
    fmodel->updateMesh(model);
    connect( fmodel, &FaceModel::metaUpdated, [=](){ this->setUnsaved(fmodel);});
    connect( fmodel, &FaceModel::meshUpdated, [=](){ this->setUnsaved(fmodel);});
    emit finishedLoad( fmodel);
}   // end postProcessLoad


// public
void FaceModelManager::setActive( FaceModel* fmodel, bool enable)
{
    const std::string& fname = fmodel->getFilePath();
    _active.erase(fname);
    if ( enable)
        _active.insert(fname);
    checkEnabledActions();
}   // end setActive


// public
void FaceModelManager::setUnsaved( FaceModel* fmodel)
{
    _unsaved.insert(fmodel->getFilePath());
    checkEnabledActions();
}   // end setUnsaved


// private
void FaceModelManager::checkEnabledActions()
{
    _saveAction.setEnabled( !_unsaved.empty());
    _saveAsAction.setEnabled( !_active.empty());
    _closeAction.setEnabled( !_active.empty());
}   // end checkEnabledActions


// private slot
void FaceModelManager::doLoad()
{
    QString anyFilter = "Any file (*.*)";
    QStringList filters = _iohandler.createImportFilters(false).split(";;");
    filters.prepend(anyFilter);
    QString allFilters = filters.join(";;");

    // Don't use native dialog because there's some Windows 10 debug output stating that
    // some element of the dialog couldn't be found. On some Win10 machines, crashes occur
    // unless non-native dialogs are used.
    QStringList fileNames = QFileDialog::getOpenFileNames( NULL,
                            tr("Load one or more models"), "", allFilters, &anyFilter, QFileDialog::DontUseNativeDialog);

    foreach ( const QString& fname, fileNames)
    {
        std::string infile = fname.toStdString();
        if ( !infile.empty())
        {
            if ( !load( infile))
            {
                std::ostringstream oss;
                oss << "Failed to load '" << fname.toStdString() << "'\n" << _err;
                QMessageBox::warning( NULL, tr("Load Failed!"), tr(oss.str().c_str()));
            }   // end if
        }   // end if
    }   // end foreach
}   // end doLoad


// private
void FaceModelManager::warnSaveFail( const std::string& fname) const
{
    std::ostringstream oss;
    oss << "Failed to save '" << fname << "'\n" << _err;
    QMessageBox::warning( NULL, tr("Save Failed!"), tr(oss.str().c_str()));
}   // end warnSaveFail


// private slot
void FaceModelManager::doSave()
{
    boost::unordered_set<std::string> fnames = _unsaved;    // Copy out
    foreach ( const std::string& fname, fnames)
    {
        FaceModel* fmodel = _openModels.at(fname);
        std::string outfile = fmodel->getFilePath();
        assert( fname == outfile);

        bool success = false;
        // If the current file path for this model is not the preferred format (.3df),
        // do save as to get the user to save into the correct format.
        if ( FaceTools::getExtension( outfile) != _iohandler.getPreferredExt())
            success = saveAs( fmodel, outfile);
        else
            success = save( fmodel, outfile);

        if ( !success)
            warnSaveFail( outfile);
    }   // end foreach
}   // end doSave


// private
bool FaceModelManager::saveAs( FaceModel* fmodel, std::string& outfile)
{
    boost::filesystem::path outpath(outfile);
    const QString parentDir = outpath.parent_path().string().c_str();

    // Make outfile have the preferred extension
    const QString dsuff = _iohandler.getPreferredExt().c_str();
    outfile = outpath.replace_extension(dsuff.toStdString()).string();

    QFileDialog fileDialog;
    fileDialog.setWindowTitle( tr("Save model"));
    fileDialog.setFileMode( QFileDialog::AnyFile);
    fileDialog.setNameFilters( _iohandler.createExportFilters().split(";;"));
    fileDialog.setDirectory( parentDir);    // Default save directory is last save location for model
    fileDialog.setDefaultSuffix( dsuff);
    fileDialog.selectFile( outfile.c_str());
    fileDialog.setAcceptMode( QFileDialog::AcceptSave);
    fileDialog.setOption( QFileDialog::DontUseNativeDialog);

    QStringList fileNames;
    if ( fileDialog.exec())
        fileNames = fileDialog.selectedFiles();

    outfile = "";
    if ( !fileNames.empty())
        outfile = fileNames.first().toStdString();
    if ( outfile.empty())
        return true;

    setUnsaved( fmodel);
    return save( fmodel, outfile);
}   // end saveAs


// private slot
void FaceModelManager::doSaveAs()
{
    assert( !_active.empty());
    boost::unordered_set<std::string> fnames = _active;    // Copy out
    foreach ( const std::string& fname, fnames)
    {
        std::string outfile = fname;
        FaceModel* fmodel = _openModels.at(fname);
        if ( !saveAs( fmodel, outfile))
            warnSaveFail( outfile);
    }   // end foreach
}   // end doSaveAs


// private slot
void FaceModelManager::doClose()
{
    while ( !_active.empty())   // Close the active set
    {
        const std::string& fname = *_active.begin();
        FaceModel* fmodel = _openModels.at(fname);
        if ( _unsaved.count(fname) > 0) // Check for unsaved changes
        {
            QString caption = (std::string("Save changes on '") + fname + "' before closing?").c_str();
            if ( QMessageBox::Yes == QMessageBox::question( NULL, "Unsaved changes!", caption, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
                save( fmodel, fname);
        }   // end if
        closeModel( fmodel);
    }   // end foreach
}   // end doClose
