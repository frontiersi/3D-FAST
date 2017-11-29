#ifndef FACE_APP_STATIC_PLUGIN_OBJ_FILE_IO_H
#define FACE_APP_STATIC_PLUGIN_OBJ_FILE_IO_H

#include <FileIOInterface.h> // FaceTools
#include <OBJExporter.h>    // RModelIO
#include <FaceModel.h>

namespace FaceApp {

class OBJFileIO : public FaceTools::FileIOInterface
{ Q_OBJECT
public:
    OBJFileIO();
    virtual ~OBJFileIO(){}

    virtual size_t getFileExtensions( std::vector<std::string>& exts) const;
    virtual std::string getFileDescription() const;
    virtual bool canImport() const { return true;}
    virtual bool canExport() const { return true;}

protected:
    virtual FaceTools::FaceModel* load( const std::string&);
    virtual bool save( const FaceTools::FaceModel*, const std::string&);

private:
    RModelIO::OBJExporter _exporter;
};  // end class

}   // end namespace

#endif


