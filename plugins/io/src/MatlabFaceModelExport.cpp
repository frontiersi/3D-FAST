#include "io/MatlabFaceModelImport.h"
using FaceApp::IO::MatlabFaceModelImport;

typedef unsigned char byte;
typedef std::pair<std::string, mxArray*> FieldPair;


class MatlabConverter
{
public:
    explicit MatlabConverter( const FaceApp::FaceModel::Ptr fm);
    ~MatlabConverter();

    bool write();
    bool openWriteFile( const std::string& fname);
    bool populateFields();

    inline const std::string& getError() const { return _errMsg;}

private:
    const FaceApp::FaceModel::Ptr _fmodel;
    MATFile* _mfile;
    boost::unordered_map<std::string, mxArray*> _fields;
    std::vector<const char*> _fieldNames;
    std::string _errMsg;

    void setFields();
    bool createObjectGeometryArrays();
    bool createTextureArrays();
    bool createImageStruct();
    bool createBorderStruct();
    bool createLandmarksStruct();
};  // end class


// public
MatlabConverter::MatlabConverter( const FaceApp::FaceModel::Ptr fm) : _fmodel(fm), _mfile(NULL)
{
    // Non-NULL fields included for backwards compatibility with existing Matlab application
    _fields["Type"] = mxCreateString( "meshObj");
    _fields["Vertices"] = NULL;
    _fields["Faces"] = NULL;
    _fields["TextureColor"] = NULL;
    _fields["ViewMode"] = mxCreateString( "Solid");
    _fields["Border"] = NULL;
    _fields["TextureMap"] = NULL;
    _fields["UV"] = NULL;
    _fields["PoseLM"] = NULL;
    _fields["ColorMode"] = mxCreateString( "Texture");
    _fields["MarkerSize"] = mxCreateDoubleScalar( 5);
    _fields["Alpha"] = mxCreateDoubleScalar( 1);
    _fields["Material"] = mxCreateString( "Default");
    _fields["LightMode"] = mxCreateString( "gouraud");

    // New fields for FaceApp::FaceModel
    _fields["ModelFile"] = mxCreateString( _fmodel->getModelFile().c_str());
    _fields["CaptureTS"] = mxCreateDoubleScalar( _fmodel->getCaptureTimeStamp());

    BOOST_FOREACH ( const FieldPair& fp, _fields)
        _fieldNames.push_back(fp.first.c_str());
}   // end ctor


MatlabConverter::~MatlabConverter()
{
    if ( _mfile)
        matClose( _mfile);
}   // end dtor


// public
bool MatlabConverter::populateFields()
{
    _errMsg = "";
    bool okay = createObjectGeometryArrays();
    if ( okay)
        okay = createTextureArrays();
    if ( okay)
        okay = createImageStruct();
    if ( okay)
        okay = createBorderStruct();
    if ( okay)
        okay = createLandmarksStruct();
    return okay;
}   // end populateFields


// public
bool MatlabConverter::openWriteFile( const std::string& fname)
{
    _mfile = matOpen( fname.c_str(), "w");
    if ( !_mfile)
    {
        _errMsg = "Unable to open " + fname + " for writing! Do you have write permission?";
        return false;
    }   // end if
    return true;
}   // end openWriteFile


// public
bool MatlabConverter::write()
{
    if ( !_mfile)
    {
        _errMsg = "No valid MATFile set! Have you called MatlabConverter::openWriteFile?";
        return false;
    }   // end if

    // Set all the fields
    mxArray* topStruct = mxCreateStructMatrix( 1, 1, _fieldNames.size(), &_fieldNames[0]);
    if ( !topStruct)
    {
        _errMsg = "Unable to allocate heap memory for top level Matlab struct on MatlabConverter::write()!";
        return false;
    }   // end if

    BOOST_FOREACH ( const FieldPair& fp, _fields)
        mxSetField( topStruct, 0, fp.first.c_str(), fp.second);

    // And write to file
    const int writeStatus = matPutVariable( _mfile, "struct", topStruct);
    if ( writeStatus != 0)
        _errMsg = "Error on file write!";

    mxDestroyArray( topStruct);
    return writeStatus == 0;
}   // end write


// private
bool MatlabConverter::createObjectGeometryArrays()
{
    const RFeatures::ObjModel::Ptr& objModel = _fmodel->getObjectModel();
    mxArray* &vtxArray = _fields["Vertices"];
    mxArray* &facesArray = _fields["Faces"];

    // Note that ALL vertices (including duplicates to avoid texturing issues) are written out
    const int nvtxs = objModel->getNumVertices();
    const int nfaces = objModel->getNumFaces();
    vtxArray = mxCreateDoubleMatrix( 3, nvtxs, mxREAL); // Data stored in column order
    facesArray = mxCreateDoubleMatrix( RFeatures::ObjPoly::NVTX, nfaces, mxREAL);  // Data in column order
    if ( !vtxArray || !facesArray)
    {
        _errMsg = "Unable to allocate heap memory to vertex and/or faces array!";
        return false;
    }   // end if

    // Store out the vertex coordinates into vtxArray in column order
    double *vptr = (double*)mxGetData( vtxArray);
    for ( int i = 0; i < nvtxs; ++i)
    {
        const cv::Vec3f& vtx = objModel->getVertex(i);
        vptr[0] = vtx[0];
        vptr[1] = vtx[1];
        vptr[2] = vtx[2];
        vptr += 3;
    }   // end for

    // Store out the vertex indices for each of the faces in column order.
    // Note that vertex indices in the MAT file are 1 based (not 0 based).
    double *fptr = (double*)mxGetData( facesArray);
    for ( int i = 0; i < nfaces; ++i)
    {
        const RFeatures::ObjPoly* face = objModel->getFace(i);
        assert(face);
        for ( int j = 0; j < RFeatures::ObjPoly::NVTX; ++j)
            fptr[j] = face->vindices[j] + 1;
        fptr += RFeatures::ObjPoly::NVTX;
    }   // end for

    return true;
}   // end createObjectGeometryArrays


// private
bool MatlabConverter::createTextureArrays()
{
    const RFeatures::ObjModel::Ptr& objModel = _fmodel->getObjectModel();
    mxArray* &texColArr = _fields["TextureColor"];
    mxArray* &uvArr = _fields["UV"];

    const int nvtxs = objModel->getNumVertices();
    const cv::Mat& img = objModel->getTexture();
    assert( img.type() == CV_8UC1 || img.type() == CV_8UC3);
    const int imgChannels = img.channels();

    const int imgRows = img.rows;
    const int imgCols = img.cols;
    const std::vector<cv::Vec2f>& uvs = objModel->getTextures();
    assert( nvtxs == uvs.size());   // Must be a texture UV offset for every vertex

    texColArr = mxCreateDoubleMatrix( 3, nvtxs, mxREAL); // RGB value in [0,1] for each vertex sampled from image
    uvArr = mxCreateDoubleMatrix( 2, nvtxs, mxREAL);    // UV offsets (V is inverted for MAT file)
    if ( !texColArr || !uvArr)
    {
        _errMsg = "Unable to allocate heap memory to texColArr and/or uvArr!";
        return false;
    }   // end if
    double* texColPtr = (double*)mxGetData(texColArr);   // Stored column wise
    double* uvPtr = (double*)mxGetData(uvArr);   // Stored column wise

    for ( int i = 0; i < nvtxs; ++i)
    {
        const cv::Vec2f& uv = uvs[i];
        uvPtr[0] = uv[0];
        uvPtr[1] = 1.0 - uv[1];   // Texture V offset is inverted for Matlab

        // Sample the colour at uv in the image
        if ( imgChannels == 1)
        {
            const double v = double( img.at<byte>(uv[1] * (imgRows-1), uv[0] * (imgCols-1)))/255;
            texColPtr[0] = v;
            texColPtr[1] = v;
            texColPtr[2] = v;
        }   // end if
        else
        {
            assert( imgChannels == 3);
            const cv::Vec3b& v = img.at<cv::Vec3b>(uv[1] * (imgRows-1), uv[0] * (imgCols-1));
            texColPtr[0] = double(v[2])/255;    // Red in [0,1]
            texColPtr[1] = double(v[1])/255;    // Green in [0,1]
            texColPtr[2] = double(v[0])/255;    // Blue in [0,1]
        }   // end else

        uvPtr += 2;
        texColPtr += 3;
    }   // end for

    return true;
}   // end createTextureArrays


// private
bool MatlabConverter::createImageStruct()
{
    const cv::Mat& img = _fmodel->getObjectModel()->getTexture();
    mxArray* &imgStruct = _fields["TextureMap"];

    const int nchannels = img.channels();
    const int rows = img.rows;
    const int cols = img.cols;
    const mwSize dims[3] = {rows, cols, nchannels};   // Always assume three channels

    // imgStruct will have three fields
    const char* fnames[3] = {"Type", "Image", "PixInterp"};
    imgStruct = mxCreateStructMatrix( 1, 1, 3, fnames);

    mxArray* f0 = mxCreateString( "image2D");
    mxArray* f1 = mxCreateNumericArray( 3, dims, mxDOUBLE_CLASS, mxREAL);
    mxArray* f2 = mxCreateString( "*nearest");
    if ( !imgStruct || !f0 || !f1 || !f2)
    {
        _errMsg = "Unable to allocate heap memory to struct.TextureMap fields!";
        return false;
    }   // end if

    // Copy in the image data (Matlab requires storage in column order, each channel separately
    double* outData = (double*)mxGetData(f1);
    for ( int k = nchannels-1; k >= 0; --k) // Go in RGB order for Matlab (OpenCV stores in BGR byte order)
    {
        for ( int j = 0; j < cols; ++j)
        {
            const byte* imgPtr = (byte*)(img.ptr() + j*nchannels + k);  // Initial row of column for channel k
            for ( int i = 0; i < rows; ++i)
            {
                *outData = double(*imgPtr)/255;
                imgPtr += cols*nchannels;
                outData++;
            }   // end for - rows
        }   // end for - cols
    }   // end for - colour channels

    mxSetField( imgStruct, 0, fnames[0], f0);
    mxSetField( imgStruct, 0, fnames[1], f1);
    mxSetField( imgStruct, 0, fnames[2], f2);

    return true;
}   // end createImageStruct


// private
bool MatlabConverter::createBorderStruct()
{
    const std::vector<int>& bvis = _fmodel->getBoundaryUniqueVertices();
    mxArray* &bstruct = _fields["Border"];

    assert( !bvis.empty());

    const char* fnames[9] = {"Type", "VerticesIndex", "ViewMode", "SingleColor",
                    "ColorMode", "Alpha", "Material", "LightMode", "MarkerSize"};
    bstruct = mxCreateStructMatrix( 1, 1, 9, fnames);
    if ( !bstruct)
    {
        _errMsg = "Unable to allocate heap memory to struct.Border!";
        return false;
    }   // end if

    mxArray* fvals[9];
    fvals[0] = mxCreateString( "borderObj");    // Type
    fvals[1] = mxCreateDoubleMatrix( 1, bvis.size(), mxREAL);   // VerticesIndex
    if ( fvals[1])
    {
        double* bptr = (double*)mxGetData(fvals[1]);
        const int nbvs = bvis.size();
        for ( int i = 0; i < nbvs; ++i)
            bptr[i] = bvis.at(i)+1;    // Vertex indices are 1 based for Matlab (0 based for ObjModel)
    }   // end if
    fvals[2] = mxCreateString( "Points");   // ViewMode
    fvals[3] = mxCreateDoubleMatrix( 1, 3, mxREAL); // SingleColor
    if ( fvals[3])
    {
        double* singleColour = (double*)mxGetData(fvals[3]);
        memset( singleColour, 0, 3*sizeof(double));
        singleColour[0] = 0.5;
    }   // end if

    fvals[4] = mxCreateString( "Single");   // ColorMode
    fvals[5] = mxCreateDoubleScalar( 1);    // Alpha
    fvals[6] = mxCreateString( "Default");  // Material
    fvals[7] = mxCreateString( "flat");     // LightMode
    fvals[8] = mxCreateDoubleScalar( 5);    // MarkerSize

    for ( int i = 0; i < 9; ++i)
    {
        if ( !fvals[i])
        {
            _errMsg = "Unable to allocate heap memory to struct.Border field!";
            return false;
        }   // end if
        mxSetField( bstruct, 0, fnames[i], fvals[i]);
    }   // end for

    return true;
}   // end createBorderStruct


// private
bool MatlabConverter::createLandmarksStruct()
{
    mxArray* &lmstruct = _fields["PoseLM"];
    const char* fnames[12] = {"Type", "Vertices", "ViewMode", "Vi", "Fi", "Names", "SingleColor",
                              "ColorMode", "Alpha", "Material", "LightMode", "MarkerSize"};
    lmstruct = mxCreateStructMatrix( 1, 1, 12, fnames);
    if ( !lmstruct)
    {
        _errMsg = "Unable to allocate heap memory to struct.PoseLM!";
        return false;
    }   // end if

    const int nlms = _fmodel->getNumLandmarks();

    mxArray* fvals[11];
    fvals[0] = mxCreateString( "LMObj");    // Type
    fvals[2] = mxCreateString( "Points");   // ViewMode

    fvals[1] = mxCreateDoubleMatrix( 3, nlms, mxREAL);  // Vertices
    fvals[3] = mxCreateDoubleMatrix( 1, nlms, mxREAL);  // Vi
    fvals[4] = mxCreateDoubleMatrix( 1, nlms, mxREAL);  // Fi
    if ( !fvals[1] || !fvals[3] || !fvals[4])
    {
        _errMsg = "Unable to allocate heap memory for number of landmarks!";
        return false;
    }   // end if

    double* lmptr = (double*)mxGetData(fvals[1]);
    double* vptr = (double*)mxGetData(fvals[3]);
    double* fptr = (double*)mxGetData(fvals[4]);
    int i = 0;
    std::vector<const char*> lmNamesArr(nlms);
    const boost::unordered_set<std::string>& lmnames = _fmodel->getLandmarkNames();
    BOOST_FOREACH ( const std::string& lmname, lmnames)
    {
        const cv::Vec3f& lmv = _fmodel->getLandmark(lmname)->getPosition();
        lmptr[0] = lmv[0];
        lmptr[1] = lmv[1];
        lmptr[2] = lmv[2];

        // Matlab indices are 1 based, not 0 based
        vptr[i] = _fmodel->getViewModel()->getClosestUniqueVertex( lmv) + 1;
        fptr[i] = vptr[i];  // Landmarks are restricted to vertices
        lmNamesArr[i++] = lmname.c_str();

        lmptr += 3;
    }   // end foreach

    fvals[5] = mxCreateCharMatrixFromStrings( nlms, &lmNamesArr[0]);    // Landmark names

    fvals[6] = mxCreateDoubleMatrix( 1, 3, mxREAL); // SingleColor
    if ( fvals[5])
    {
        double* singleColour = (double*)mxGetData(fvals[5]);
        memset( singleColour, 0, 3*sizeof(double));
        singleColour[2] = 1;
    }   // end if

    fvals[7] = mxCreateString( "Single");   // ColorMode
    fvals[8] = mxCreateDoubleScalar( 1);    // Alpha
    fvals[9] = mxCreateString( "Default");  // Material
    fvals[10] = mxCreateString( "flat");    // LightMode
    fvals[11] = mxCreateDoubleScalar( 20);  // MarkerSize

    for ( int i = 0; i < 11; ++i)
    {
        if ( !fvals[i])
        {
            _errMsg = "Unable to allocate heap memory to struct.PoseLM field!";
            return false;
        }   // end if
        mxSetField( lmstruct, 0, fnames[i], fvals[i]);
    }   // end for

    return true;
}   // end createLandmarksStruct


MatlabFaceModelImport::MatlabFaceModelImport( const FaceApp::FaceModel::Ptr& fm)
    : _fmodel(fm)
{}   // end ctor


// public
bool MatlabFaceModelImport::save( const std::string& fname) const
{
    MatlabConverter mconverter( _fmodel);

    if ( !mconverter.openWriteFile( fname))
    {
        _errMsg = mconverter.getError();
        return false;
    }   // end if

    if ( !mconverter.populateFields())
    {
        _errMsg = mconverter.getError();
        return false;
    }   // end if

    if ( mconverter.write())
    {
        _errMsg = mconverter.getError();
        return false;
    }   // end if

    return true;
}   // end save


// protected
bool MatlabFaceModelImport::write( std::ostream& os) const
{
    // Matlab does not currently provide a C++ streams style API so can't use this interface!
    throw FaceApp::IO::IOException(
            "MatlabFaceModelImport::write is not supported. Use MatlabFaceModelImport::save instead.");
    return false;   // Req for compiler warning
}   // end write
