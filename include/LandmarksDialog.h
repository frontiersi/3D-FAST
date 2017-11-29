#ifndef FACE_APP_LANDMARKS_DIALOG_H
#define FACE_APP_LANDMARKS_DIALOG_H

#include <QDialog>
#include <QStandardItem>
#include <FaceControl.h>  // FaceTools

namespace Ui { class LandmarksDialog; }
namespace FaceApp {

class LandmarksDialog : public QDialog
{ Q_OBJECT
public:
    explicit LandmarksDialog(QWidget *parent=0);
    ~LandmarksDialog() override;

    void setModel( FaceTools::FaceControl* fcont=NULL); // Update table of landmarks for the given model.

protected:
    void showEvent( QShowEvent*) override;

private slots:
    void doOnLandmarkHighlighted( std::string lm, const QPoint&);
    void doOnItemChanged(QStandardItem*);
    void resetLandmarks();

private:
    Ui::LandmarksDialog *ui;
    FaceTools::FaceControl* _fcont;

    boost::unordered_map<std::string, int> _markerRows;
    QStandardItem* getRowItem( const std::string&, int col);
    std::string getLandmarkFromRowId( int) const;

    void changeLandmarkName( const std::string&, const std::string&);
    void setMarkerVisible( const std::string&, bool);
    void setMarkerMovable( const std::string&, bool);
    void setMarkerDeletable( const std::string&, bool);

    void clear();
    void removeRow( const std::string&);
    void appendRow( const std::string&);
    void createRow( const std::string&, QList<QStandardItem*>&);

    void connectModel();

    LandmarksDialog( const LandmarksDialog&);   // NO COPY
    void operator=( const LandmarksDialog&);    // NO COPY
};  // end class

}   // end namespace

#endif
