#ifndef FACE_APP_ABOUT_DIALOG_H
#define FACE_APP_ABOUT_DIALOG_H

#include <QDialog>

namespace Ui { class AboutDialog;}
namespace FaceApp {

class AboutDialog : public QDialog
{ Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog() override;

private:
    Ui::AboutDialog *ui;

    void insertHeader( const QString&);
    void appendPara( const QString&);
    void finishContent();
    void insertContent();
    AboutDialog( const AboutDialog&);
    void operator=( const AboutDialog&);
};  // end class

}   // end namespace

#endif
