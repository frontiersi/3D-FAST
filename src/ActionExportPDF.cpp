#include <QApplication>
#include <ActionExportPDF.h>
#include <InteractiveModelViewer.h>
#include <FaceControl.h>
#include <FaceModel.h>
#include <PDFGenerator.h>      // RModelIO
#include <U3DExporter.h>       // RModelIO
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <boost/filesystem.hpp>
#include <fstream>
#include <cassert>
using FaceApp::ActionExportPDF;
using FaceTools::FaceControl;
using FaceTools::ModelViewer;
using RModelIO::PDFGenerator;
using RModelIO::U3DExporter;
using RFeatures::CameraParams;
using RFeatures::ObjModel;

// public
ActionExportPDF::ActionExportPDF( const std::string& dname, const std::string& fname, QProgressBar* pbar)
    : FaceTools::FaceAction(),
      _icon(fname.c_str()), _dname(dname.c_str()),
      _pupdater( pbar, 1),
      _workdir( boost::filesystem::path( qApp->applicationDirPath().toStdString()))
{
    init();
    setAsync( true, &_pupdater);
    connect( this, &FaceAction::finished, this, &ActionExportPDF::finishedPDF);
    std::cerr << "Location of IDTFConverter: " << RModelIO::U3DExporter::IDTFConverter << " ("
              << (RModelIO::U3DExporter::isAvailable()  ? "" : "not ") << "available)" << std::endl;
    std::cerr << "Location of pdflatex:      " << RModelIO::PDFGenerator::pdflatex << " ("
              << (RModelIO::PDFGenerator::isAvailable() ? "" : "not ") << "available)" << std::endl;
    setEnabled( false);
}   // end ctor


// private slot
void ActionExportPDF::finishedPDF( bool v)
{
    if ( !_pdffile.empty())
    {
        if ( v)
        {
            std::cerr << "Created PDF at '" << _pdffile << "'" << std::endl;
            QMessageBox::information( NULL, tr("Created PDF"), tr( (std::string("Created PDF at '") + _pdffile + "'").c_str()));
        }   // end if
        else 
            QMessageBox::warning( NULL, tr("PDF Export Error!"), tr( _pdffile.c_str()));
    }   // end if
    _pupdater.reset();
    _pdffile = "";
}   // end finishedPDF


// public
bool ActionExportPDF::setWorkingDir( const std::string& workdir)
{
    bool setwdir = false;
    boost::filesystem::path wdir = boost::filesystem::path(workdir);
    if ( boost::filesystem::is_directory( wdir))
    {
        _workdir = wdir;
        setwdir = true;
    }   // end if
    return setwdir;
}   // end setWorkingDir


// public
bool ActionExportPDF::setLogo( const std::string& logofile)
{
    bool gotlogo = true;
    _logopath = boost::filesystem::path(logofile);
    if ( !boost::filesystem::exists( _logopath))
    {
        _logopath = boost::filesystem::path();
        gotlogo = false;
    }   // end if
    return gotlogo;
}   // end setLogo


// public
void ActionExportPDF::setControlled( FaceControl* fcont, bool enable)
{
    assert(fcont);
    _fconts.erase(fcont);
    if ( enable)
        _fconts.insert(fcont);
    setEnabled( _fconts.size() == 1 && RModelIO::PDFGenerator::isAvailable() && RModelIO::U3DExporter::isAvailable());
}   // end setControlled


namespace
{

bool writeDoc( RModelIO::PDFGenerator::LaTeXU3DInserter* fig, const std::string& texfile, const std::string& logofile)
{
    assert(fig);
    bool writtenOkay = false;
    std::ofstream os;
    try
    {
        os.open( texfile.c_str(), std::ios::out);
        os << "\\documentclass{article}" << std::endl;
        os << "\\usepackage[a4paper]{geometry}" << std::endl;
        os << "\\usepackage{a4wide}" << std::endl;
        os << "\\usepackage{media9}" << std::endl;
        os << "\\usepackage[parfill]{parskip}" << std::endl;
        os << "\\usepackage[colorlinks=true,urlcolor=red]{hyperref}" << std::endl;
        os << "\\usepackage{graphicx}" << std::endl;
        os << "\\DeclareGraphicsExtensions{.png,.jpg,.pdf}" << std::endl;
        os << std::endl;
        os << "\\begin{document}" << std::endl;
        os << "\\title{3D-FAST: Facial Analysis Report (beta)}" << std::endl;
        os << "\\author{\\href{mailto:r.l.palmer@curtin.edu.au?subject=3D-FAST PDF report (beta)}{Contact Developers}}" << std::endl;
        os << "\\maketitle" << std::endl;
        os << "\\thispagestyle{empty}" << std::endl;
        os << std::endl;

        os << *fig << std::endl;

        if ( !logofile.empty())
        {
            os << "\\begin{figure}[!ht]" << std::endl;
            os << "\\centering" << std::endl;
            os << "\\includegraphics[]{" << logofile << "}" << std::endl;
            os << "\\end{figure}" << std::endl;
        }   // end if

        os << "\\end{document}" << std::endl;
        os.flush();
        writtenOkay = true;
    }   // end try
    catch ( const std::exception& e)
    {
        std::cerr << "[EXCEPTION!] Exception writing to file " << texfile << std::endl;
        std::cerr << e.what() << std::endl;
        writtenOkay = false;
    }   // end catch
    os.close();
    return writtenOkay;
}   // end writeDoc

}   // end namespace


// public virtual
void ActionExportPDF::doPrepAction()
{
    assert( _fconts.size() == 1);
    const FaceControl* fcont = *_fconts.begin();
    assert( fcont->getViewer());
    std::string outfile = fcont->getModel()->getObjectMeta()->getObjectFile();
    boost::filesystem::path outpath(outfile);
    const QString parentDir = outpath.parent_path().string().c_str();

    // Make _outfile have the preferred extension
    outfile = outpath.replace_extension( "pdf").string();

    QFileDialog fileDialog;
    fileDialog.setWindowTitle( tr("Export as PDF"));
    fileDialog.setFileMode( QFileDialog::AnyFile);
    fileDialog.setNameFilter( "Portable Document Format (*.pdf)");
    fileDialog.setDirectory( parentDir);    // Default save directory is last save location for model
    fileDialog.setDefaultSuffix( "pdf");
    fileDialog.selectFile( outfile.c_str());
    fileDialog.setAcceptMode( QFileDialog::AcceptSave);
    fileDialog.setOption( QFileDialog::DontUseNativeDialog);

    QStringList fileNames;
    if ( fileDialog.exec())
        fileNames = fileDialog.selectedFiles();
    fileDialog.close();

    _pdffile = "";
    if ( !fileNames.empty())
        _pdffile = fileNames.first().toStdString();
}   // end doPrepAction


// protected
bool ActionExportPDF::doAction()
{
    if ( _pdffile.empty())
        return true;

    std::cerr << "[INFO] FaceApp::ActionExportPDF::doAction: Commencing PDF generation..." << std::endl;
    assert( _fconts.size() == 1);
    const FaceControl* fcont = *_fconts.begin();

    _pupdater.reset();
    _pupdater.processUpdate(0.01f);

    const std::string texpath = (_workdir / "main.tex").string();

    std::string err;

    // Currently, the model itself is written out, not the VTK view (next version!)
    const ModelViewer* viewer = fcont->getViewer();
    const ObjModel::Ptr model = fcont->getModel()->getObjectMeta()->getObject();
    RModelIO::PDFGenerator pdfgen;
    _pupdater.processUpdate(0.2f);
    RModelIO::PDFGenerator::LaTeXU3DInserter* fig = pdfgen.getFigureInserter( model, 150, 150, viewer->getCamera(), "3D Facial Image (beta)");
    if ( !fig)
        err = "Failed to create U3D model from data!";
    _pupdater.processUpdate(0.6f);

    if ( err.empty())
    {
        if ( !writeDoc( fig, texpath, _logopath.string()))
            err = "Failed to create '" + texpath + "' for LaTeX parsing!";
    }   // end if
    _pupdater.processUpdate(0.65f);

    if ( err.empty())
    {
        if ( !pdfgen( texpath, true))
            err = "Failed to generate PDF!";
    }   // end if
    _pupdater.processUpdate(0.9f);

    const boost::filesystem::path pdfout( _workdir / "main.pdf");
    if ( err.empty() && boost::filesystem::exists( pdfout))
    {
        // Copy the created pdf to the requested location
        if ( boost::filesystem::exists( _pdffile))
            boost::filesystem::remove( _pdffile);
        boost::filesystem::copy( pdfout, _pdffile);
        boost::filesystem::remove( pdfout);
    }   // end if
    else
        _pdffile = err;

    _pupdater.processUpdate(1.0f);
    return err.empty();
}   // end doAction
