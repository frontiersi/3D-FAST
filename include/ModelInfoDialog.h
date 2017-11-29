#ifndef FACE_APP_MODEL_INFO_DIALOG_H
#define FACE_APP_MODEL_INFO_DIALOG_H

#include <QDialog>
#include <FaceModel.h>

namespace Ui { class ModelInfoDialog;}
namespace FaceApp {

class ModelInfoDialog : public QDialog
{ Q_OBJECT
public:
    explicit ModelInfoDialog( QWidget *parent=0);
    ~ModelInfoDialog() override;

    void setModel( const FaceTools::FaceModel* fm=NULL); // Clears by default
    void refresh(); // Refresh data from current model

private:
    Ui::ModelInfoDialog *ui;
    const FaceTools::FaceModel *_fmodel;

    void clear();
    ModelInfoDialog( const ModelInfoDialog&);   // NO COPY
    void operator=( const ModelInfoDialog&);    // NO COPY
};  // end class

}   // end namespace

#endif
