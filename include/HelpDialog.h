#ifndef FACE_APP_HELP_DIALOG_H
#define FACE_APP_HELP_DIALOG_H

#include <QDialog>

namespace Ui { class HelpDialog;}
namespace FaceApp {

class HelpDialog : public QDialog
{ Q_OBJECT
public:
    explicit HelpDialog( QWidget *parent = 0);
    ~HelpDialog() override;

private:
    Ui::HelpDialog *ui;
    HelpDialog( const HelpDialog&);
    void operator=( const HelpDialog&);
};  // end class

}   // end namespace

#endif
