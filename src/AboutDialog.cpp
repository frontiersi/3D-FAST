#include <AboutDialog.h>
#include <ui_AboutDialog.h>
using FaceApp::AboutDialog;
#include "3dfast_Config.h"


void AboutDialog::insertHeader( const QString& title)
{
    QStringList cnt;
    cnt << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
        << "<html><head><meta name=\"qrichtext\" content=\"1\" />"
        << "<style type=\"text/css\">"
        << "p, li { white-space: pre-wrap; }"
        << "</style></head>"
        << "<body style=\"font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\">"
        << "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
        << "<span style=\"font-size:11pt; font-weight:600;\">"
        << title
        << "</span>"
        << "<br/>"
        << "</p>";
    ui->textBrowser->insertHtml( cnt.join('\n'));
}   // end insertHeader


void AboutDialog::finishContent()
{
    QStringList appv;
    appv << QString("%0").arg(APP_VERSION_MAJOR);
    appv << QString("%0").arg(APP_VERSION_MINOR);
    appv << QString("%0").arg(APP_VERSION_PATCH);
    appv << QString("%0").arg(APP_BUILD_DTSTAMP);
    QString appString = appv.join('.');

    QStringList appn;
    appn << APP_NAME << QString::fromWCharArray(L"\u00A9 2017") << APP_AUTHOR_NAME
         << QString("(<a href=\"mailto:%0?Subject=%1 (BUG REPORT) version %2\">report bugs</a>)").arg( APP_AUTHOR_EMAIL, APP_NAME, appString);

    QStringList cnt;
    cnt << "<br/>"
        << "<p align=\"right\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
        << "<img src=\":/logos/logo_big.png\"/>"
        << "<br/>"
        << appn.join(' ')
        << "<br/>"
        << "Version " << appString
        << "<br/>"
        << "<a href=\"https://github.com/richeytastic/3D-FAST\">" << APP_NAME << "is open source</a>"
        << "</p>"
        << "</body>"
        << "</html>";
    ui->textBrowser->insertHtml( cnt.join('\n'));
}   // end finishContent


void AboutDialog::appendPara( const QString& para)
{
    QStringList cnt;
    cnt << "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
        << "<br/>"
        << para
        << "<br/>"
        << "</p>";
    ui->textBrowser->insertHtml( cnt.join('\n'));
}   // end appendPara


void AboutDialog::insertContent()
{
    insertHeader( APP_DESCRIPTION);

    QStringList p1, p2, p3;
    p1 << APP_NAME << "allows for visualisation and analysis of 3D facial images"
       << "as stored in a number of common file formats. Up to three different"
       << "visualisations of a face (or different faces) can be viewed in adjacent"
       << "view panes using the move and copy controls beneath the view panes."
       << "Tools to interact with the face models are available including different"
       << "visualisation formats, facial landmark detection, and cleaning / cropping."
       << "Landmarks can be freely added or removed, and a measurement tool allows for"
       << "distances between arbitrary points to be found.";
    p2 << "Save models to the" << APP_NAME << "(.3df) file format to retain a copy of the processed"
       << "model along with landmark data. The .3df file is an XML format file with references"
       << "out to model files saved in the standard Wavefront .obj format.";
    p3 << "Finally, the model may be exported as a fully 3D maipulable object inside a PDF"
       << "(features currently in beta) for sending via email etc. 3D PDFs can be viewed"
       << "using Adobe Reader (<a href=\"http://www.adobe.com/\">www.adobe.com</a>)";

    appendPara( p1.join(' '));
    appendPara( p2.join(' '));
    appendPara( p3.join(' '));

    finishContent();
}   // end insertContent


AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setModal(false);
    connect( ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    ui->textBrowser->setOpenExternalLinks(true);
    insertContent();
}   // end ctor


AboutDialog::~AboutDialog()
{
    delete ui;
}   // end dtor
