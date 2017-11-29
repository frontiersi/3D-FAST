#ifndef FACE_APP_CORE_DIALOGS_H
#define FACE_APP_CORE_DIALOGS_H

#include <FaceControl.h>    // FaceTools
#include <PluginsDialog.h>  // QTools
#include "HelpDialog.h"
#include "AboutDialog.h"
#include "OptionsDialog.h"
#include "LicensesDialog.h"
#include "ModelInfoDialog.h"
#include "LandmarksDialog.h"

namespace FaceApp {

class CoreDialogs : public QObject
{
Q_OBJECT
public:
    CoreDialogs();
    ~CoreDialogs() override;

    void hideAll();    // Hide all dialogs

    QDialog* helpDialog() { return _helpDialog;}
    QDialog* aboutDialog() { return _aboutDialog;}
    QDialog* optionsDialog() { return _optionsDialog;}
    QDialog* licensesDialog() { return _licensesDialog;}
    QDialog* landmarksDialog() { return _landmarksDialog;}
    QDialog* modelInfoDialog() { return _modelInfoDialog;}
    QTools::PluginsDialog* pluginsDialog() { return _pluginsDialog;}

    // Get the current set of options
    FaceTools::ModelOptions getOptions() const;

signals:
    void onOptionsChanged( const FaceTools::ModelOptions&);

public slots:
    void setActive( FaceTools::FaceControl* fcont=NULL);

    bool loadOptions( const std::string&);  // Load from file
    bool saveOptions( const std::string&);  // Save to file

private slots:
    void doOptionsChanged();

private:
    FaceTools::FaceControl *_fcont;

    HelpDialog *_helpDialog;
    AboutDialog *_aboutDialog;
    OptionsDialog *_optionsDialog;
    LicensesDialog *_licensesDialog;
    LandmarksDialog *_landmarksDialog;
    ModelInfoDialog *_modelInfoDialog;
    QTools::PluginsDialog *_pluginsDialog;

    void setOptionsDialogFromFaceControl( FaceTools::FaceControl*);

    CoreDialogs( const CoreDialogs&);       // No copy
    void operator=( const CoreDialogs&);    // No copy
};  // end class

}   // end namespace

#endif
