#ifndef FACE_APP_STATIC_PLUGIN_XML_FILE_IO_H
#define FACE_APP_STATIC_PLUGIN_XML_FILE_IO_H

#include <FileIOInterface.h> // FaceTools
#include <FaceModel.h>

namespace FaceApp {

const std::string XML_VERSION = "1.31";
const std::string XML_FILE_EXTENSION( "3df");
const std::string XML_FILE_DESCRIPTION( "3D-FAST");
const std::string XML_FACEMODEL( "facemodel");

const std::string XML_CAMERA( "camera");
const std::string XML_POS( "pos");
const std::string XML_FOCUS( "focus");
const std::string XML_UP( "up");
const std::string XML_FOV( "fov");

const std::string XML_X( "x");
const std::string XML_Y( "y");
const std::string XML_Z( "z");

const std::string XML_ATTR_VERSION( "<xmlattr>.version");
const std::string XML_ATTR_COUNT( "<xmlattr>.count");
const std::string XML_ATTR_NAME( "<xmlattr>.name");



class XMLFileIO : public FaceTools::FileIOInterface
{ Q_OBJECT
public:
    XMLFileIO();
    virtual ~XMLFileIO(){}

    virtual std::string getFileDescription() const { return XML_FILE_DESCRIPTION;}
    virtual size_t getFileExtensions( std::vector<std::string>& exts) const;

    virtual bool canImport() const { return true;}
    virtual bool canExport() const { return true;}

protected:
    virtual FaceTools::FaceModel* load( const std::string&);
    virtual bool save( const FaceTools::FaceModel*, const std::string&);
};  // end class


}   // end namespace FaceApp

#endif


