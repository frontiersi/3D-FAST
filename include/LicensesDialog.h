#ifndef FACE_APP_LICENSES_DIALOG_H
#define FACE_APP_LICENSES_DIALOG_H

#include <QDialog>

namespace Ui { class LicensesDialog;}
namespace FaceApp {

class LicensesDialog : public QDialog
{ Q_OBJECT
public:
    explicit LicensesDialog( QWidget *parent = 0);
    ~LicensesDialog() override;

private:
    Ui::LicensesDialog *ui;
    LicensesDialog( const LicensesDialog&);
    void operator=( const LicensesDialog&);
};  // end class

}   // end namespace

#endif
