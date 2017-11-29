#ifndef FACE_APP_FACE_MODEL_MANAGER_H
#define FACE_APP_FACE_MODEL_MANAGER_H

#include <FaceModelFileIOHandler.h>
#include <FaceModel.h>

namespace FaceApp
{

class FaceModelManager : public QObject
{ Q_OBJECT
public:
    FaceModelManager();
    ~FaceModelManager() override;

    void addFileFormat( FaceTools::FileIOInterface*);  // Forwards through to FaceTools::FaceModelIOHandler
    bool canLoad( const QString&) const;    // Returns true iff the file at the given path can be loaded.

    // Loads model and fires finishedLoad when done. Returns success in starting asynchronous operation.
    // Note that if the model is already open, finishedLoad will fire with the pointer to the existing model!
    bool load( const std::string& fname);

    // Saves model and fires finishedSave when done. Returns success in starting asynchronous operation.
    bool save( FaceTools::FaceModel*, const std::string& savefilepath);

    void closeModel( FaceTools::FaceModel*);  // Close given model (assumes created via this object) WITHOUT SAVING.
    void closeAllModels();                     // Closes all currently opened models WITHOUT SAVING.

    const std::string& getError() const { return _err;} // Error string after failed io.

    void setActive( FaceTools::FaceModel*, bool);
    void setUnsaved( FaceTools::FaceModel*);    // Will be called by FaceModels directly
    size_t getNumUnsaved() const { return _unsaved.size();}

    QAction* getLoadAction() { return &_loadAction;}        // Load one or more models.
    QAction* getSaveAction() { return &_saveAction;}        // Saves unsaved.
    QAction* getSaveAsAction() { return &_saveAsAction;}    // Saves active model (only if a single model active).
    QAction* getCloseAction() { return &_closeAction;}      // Close the active set - allowing user option to save if not.

signals:
    void startedClose( FaceTools::FaceModel*); // Notify listeners that this model is about to be closed!
    void finishedLoad( FaceTools::FaceModel*); // May supply a NULL model if error encountered.
    void finishedSave( FaceTools::FaceModel*); // May supply a NULL on error.

private slots:
    void doLoad();
    void doSave();
    void doSaveAs();
    void doClose();

    void postProcessLoad();
    void postProcessSave();

private:
    std::string _err;

    QAction _loadAction;
    QAction _saveAction;
    QAction _saveAsAction;
    QAction _closeAction;

    FaceTools::FaceModelFileIOHandler _iohandler;
    boost::unordered_map<std::string, FaceTools::FaceModel*> _openModels;
    boost::unordered_set<std::string> _unsaved;
    boost::unordered_set<std::string> _active;

    void checkEnabledActions();
    void warnSaveFail( const std::string&) const;
    bool saveAs( FaceTools::FaceModel*, std::string&);

    FaceModelManager( const FaceModelManager&);
    FaceModelManager& operator=( const FaceModelManager&);
};  // end class

}   // end namespace

#endif

