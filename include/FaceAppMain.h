#ifndef FACE_APP_FACE_APP_MAIN_H
#define FACE_APP_FACE_APP_MAIN_H

#include <PluginInterface.h>    // QTools
#include <QDragEnterEvent>
#include <QProgressBar>
#include <QMainWindow>

#include "3dfast_Config.h"
#include "CoreDialogs.h"
#include "MultiFaceModelViewer.h"
#include "FaceModelManager.h"

namespace Ui { class FaceAppMain;}

namespace FaceApp {

class FaceAppMain : public QMainWindow
{ Q_OBJECT
public:
    FaceAppMain();
    ~FaceAppMain() override;

public slots:
    bool loadModel( const QString&);

protected:
    void dragEnterEvent( QDragEnterEvent*) override;
    void dropEvent( QDropEvent*) override;
    void closeEvent( QCloseEvent *) override;
    QSize sizeHint() const override;

private slots:
    void setPlugin( QTools::PluginInterface*);
    void finishedLoad( FaceTools::FaceModel*);
    void finishedSave( FaceTools::FaceModel*);
    void setActive( FaceTools::FaceControl*, bool);

private:
    Ui::FaceAppMain *ui;
    CoreDialogs* _dialogs;
    MultiFaceModelViewer* _mviewer;
    QProgressBar* _progressBar;
    FaceModelManager _fmm;
    QMenu _contextMenu;
    QList<QAction*> _allActions;

    void loadPlugins();
    void createFileMenu();
    void createHelpMenu();
    void finishMenus();
    void finishMainToolbar();
    void setAction( FaceTools::FaceAction*);

    FaceAppMain( const FaceAppMain&);     // No copy
    void operator=( const FaceAppMain&);  // No copy
};  // end class

}   // end namespace


#endif
