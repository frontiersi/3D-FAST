#ifndef FACE_APP_STATIC_PLUGIN_MATLAB_FILE_IO_H
#define FACE_APP_STATIC_PLUGIN_MATLAB_FILE_IO_H

#include <FileIOInterface.h> // FaceTools


namespace FaceApp {

class MatlabFileIO : public FaceTools::FileIOInterface
{ Q_OBJECT
public:
    virtual ~MatlabFileIO(){}

    virtual std::string getFileDescription() const { return "Matlab binary";}
    virtual size_t getFileExtensions( std::vector<std::string>& exts) const;

    virtual bool canImport() const { return true;}
    virtual bool canExport() const { return true;}

protected:
    virtual FaceTools::FaceModel* load( const std::string& fname);
    virtual bool save( const FaceTools::FaceModel*, const std::string& fname);
};  // end class


}   // end namespace FaceApp

#endif


