#include <HelpDialog.h>
#include <ui_HelpDialog.h>
using FaceApp::HelpDialog;


HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent), ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    this->setModal(false);
    connect( ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
}   // end ctor


HelpDialog::~HelpDialog()
{
    delete ui;
}   // end dtor
