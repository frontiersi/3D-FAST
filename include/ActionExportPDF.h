#ifndef FACE_APP_ACTION_EXPORT_PDF_H
#define FACE_APP_ACTION_EXPORT_PDF_H

#include <FaceActionInterface.h>
#include <boost/filesystem/path.hpp>
#include <boost/unordered_set.hpp>

namespace FaceApp {

class ActionExportPDF : public FaceTools::FaceAction
{ Q_OBJECT
public:
    ActionExportPDF( const std::string& dname="Export PDF",
                     const std::string& iconfilename="",
                     QProgressBar* pbar=NULL);

    const QIcon* getIcon() const override { return &_icon;}
    QString getDisplayName() const override { return _dname;}

    void setControlled( FaceTools::FaceControl*, bool) override;

    bool setWorkingDir( const std::string& wdir);   // Returns true iff directory exists and was set
    bool setLogo( const std::string& logoFile);     // Returns true iff logo file exists and was set

protected:
    void doPrepAction() override;
    bool doAction() override;

private slots:
    void finishedPDF( bool);

private:
    const QIcon _icon;
    const QString _dname;
    boost::unordered_set<FaceTools::FaceControl*> _fconts;

    QTools::QProgressUpdater _pupdater;
    boost::filesystem::path _workdir;
    boost::filesystem::path _logopath;
    std::string _pdffile;
};  // end class

}   // end namespace

#endif
