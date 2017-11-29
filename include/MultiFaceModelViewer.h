#ifndef FACE_APP_MULTI_FACE_MODEL_VIEWER_H
#define FACE_APP_MULTI_FACE_MODEL_VIEWER_H

#include <FaceModelViewer.h>    // FaceTools
#include <FaceControl.h>        // FaceTools
#include <QToolButton>
#include <QVBoxLayout>
#include <QSplitter>

namespace FaceApp
{

class MultiFaceModelViewer : public QWidget
{ Q_OBJECT
public:
    MultiFaceModelViewer( const QList<QAction*> *actions=NULL,  // Actions available to every view
                          QMenu* contextMenu=NULL,              // Context menu available from every viewer
                          QWidget *parent=NULL);
    ~MultiFaceModelViewer() override;

public slots:
    // Toggle visibility of left/right viewers
    void setLeftViewerVisible(bool);
    void setRightViewerVisible(bool);

    void insert( FaceTools::FaceModel*);    // Ensure view for the given model is present in the middle panel.
    void erase( FaceTools::FaceModel*);     // Remove all views for the given model.

    void doOnOptionsChanged( const FaceTools::ModelOptions&);

    void resetCamera(); // On the active viewer
    void saveScreenshot();  // On the active viewer

signals:
    void requestContextMenu( const QPoint&);
    void setActive( FaceTools::FaceControl*, bool);

private slots:
    void moveLeftToCentre();
    void moveCentreToLeft();
    void moveCentreToRight();
    void moveRightToCentre();

    void copyLeftToCentre();
    void copyCentreToLeft();
    void copyCentreToRight();
    void copyRightToCentre();

    void onLeftViewerUpdatedSelected( FaceTools::FaceControl*, bool);
    void onCentreViewerUpdatedSelected( FaceTools::FaceControl*, bool);
    void onRightViewerUpdatedSelected( FaceTools::FaceControl*, bool);

private:
    const QList<QAction*> *_actions;

    QAction* _moveLeftToCentreAction;
    QAction* _copyLeftToCentreAction;
    QAction* _moveRightToCentreAction;
    QAction* _copyRightToCentreAction;

    QAction* _moveCentreToLeftAction;
    QAction* _copyCentreToLeftAction;
    QAction* _moveCentreToRightAction;
    QAction* _copyCentreToRightAction;

    FaceTools::FaceModelViewer *_activeViewer;
    FaceTools::FaceModelViewer *_v0;
    FaceTools::FaceModelViewer *_v1;
    FaceTools::FaceModelViewer *_v2;
    QSplitter *_splitter;
    boost::unordered_set<FaceTools::FaceControl*> _fconts;

    void moveViews( FaceTools::FaceModelViewer*, FaceTools::FaceModelViewer*);
    void copyViews( FaceTools::FaceModelViewer*, FaceTools::FaceModelViewer*);
    void removeViews( FaceTools::FaceModelViewer*);

    void checkEnableLeftToCentre();
    void checkEnableCentreToLeft();
    void checkEnableCentreToRight();
    void checkEnableRightToCentre();
    size_t canCopyTo( FaceTools::FaceModelViewer*, FaceTools::FaceModelViewer*, boost::unordered_set<FaceTools::FaceModel*>&) const;

    void deactivateAllSelectedOnViewer( FaceTools::FaceModelViewer*);
    void addCommonButtons( QHBoxLayout*, FaceTools::FaceModelViewer*);
    QToolButton* makeButton( QAction*);

    MultiFaceModelViewer( const MultiFaceModelViewer&); // No copy
    void operator=( const MultiFaceModelViewer&);       // No copy
};  // end class

}   // end namespace

#endif

