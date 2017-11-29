#include "MatlabFaceModelImport.h"
using FaceApp::IO::MatlabFaceModelImport;


class MatlabReader
{
public:
    explicit MatlabReader( mxArray* svar) : _svar(svar) {}
    ~MatlabReader() { if ( _svar) mxDestroyArray( _svar);}

    bool readInStructContents();

    mxArray* _svar;

    RFeatures::ObjModel::Ptr _objModel;
    std::vector<int> _bvis;     // Boundary vertex indices
    std::vector<cv::Vec3f> _lmcoords;   // Landmark coordinates
    std::vector<std::string> _lmnames;  // Landmark names
    std::vector<int> _lmovis;   // Corresponding object vertices
    std::vector<int> _lmofis;   // And face indices (same as vertex IDs here)
    std::string _modelfile;
    long _capturets;

    // Map MAT indices (1 based) to ObjModel indices (0 based)
    std::vector<int> _vtxIndices;
    std::vector<int> _faceIndices;
    bool readModelGeometry();
    bool readTextureMap();
    bool readBoundaryVertices();
    bool readLandmarks();
};  // end class


FaceApp::FaceModel::Ptr createFaceModel( const MatlabReader& mr)
{
    FaceApp::FaceModel::Ptr fm = FaceApp::FaceModel::create( mr._objModel);
    fm->setModelFile( mr._modelfile);
    fm->setCaptureTimeStamp( mr._capturets);
    const int nlms = mr._lmcoords.size();
    for ( int i = 0; i < nlms; ++i)
        fm->setLandmark( mr._lmnames[i], mr._lmcoords[i]);
    fm->resetVtkModel( mr._bvis);   // Segments view model
    return fm;
}   // end createFaceModel


bool MatlabReader::readTextureMap()
{
    mxArray* textureMap = mxGetField( _svar, 0, "TextureMap");   // Get the texture map
    if ( !mxIsStruct( textureMap))
    {
        std::cerr << "Couldn't find 'TextureMap' struct" << std::endl;
        return false;
    }   // end if
    const cv::Mat texture = FaceApp::IO::createTexture( textureMap);
    if ( texture.empty())
        return false;
    _objModel->setTexture( texture);
    return true;
}   // end readTextureMap


bool MatlabReader::readBoundaryVertices()
{
    mxArray* bstruct = mxGetField( _svar, 0, "Border");
    if ( !bstruct)
    {
        std::cerr << "Unable to get Border field from struct!" << std::endl;
        return false;
    }   // end if

    _bvis.clear();

    mxArray* bindices = mxGetField( bstruct, 0, "VerticesIndex");
    if ( !bindices) // Will be NULL if no data stored
        return true;

    const double* bidx = (const double*)mxGetData( bindices);
    const mwSize* mwDims = mxGetDimensions( bindices);
    if ( mwDims[0] != 1)
    {
        std::cerr << "Error - invalid number of rows (should be 1) for struct.Border.VerticesIndex" << std::endl;
        return false;
    }   // end if
    const int nindices = mwDims[1];
    //std::cerr << "struct.Border.VerticesIndex has " << nindices << " array elements" << std::endl;
    _bvis.resize(nindices);
    for ( int i = 0; i < nindices; ++i)
        _bvis[i] = _vtxIndices[(int)bidx[i]];

    return true;
}   // end readBoundaryVertices


bool MatlabReader::readLandmarks()
{
    mxArray* lmstruct = mxGetField( _svar, 0, "PoseLM");
    if ( !lmstruct)
    {
        std::cerr << "Unable to get PoseLM field from struct!"  << std::endl;
        return false;
    }   // end if

    _lmcoords.clear();
    _lmovis.clear();
    _lmofis.clear();

    if ( !lmstruct || !mxIsStruct(lmstruct))    // Will be NULL if no data stored
        return true;

    mxArray* vtxs = mxGetField( lmstruct, 0, "Vertices");
    mxArray* ovtxIdxs = mxGetField( lmstruct, 0, "Vi");     // Obj vertex indices
    mxArray* fvtxIdxs = mxGetField( lmstruct, 0, "Fi");     // Obj face indices
    mxArray* lmnames = mxGetField( lmstruct, 0, "Names");   // Landmark names (lmnames may be NULL)

    // If parent struct is not NULL, then if these fields are NULL there is no data stored
    if ( !vtxs || !ovtxIdxs || !fvtxIdxs)
        return true;

    if ( !mxIsDouble( vtxs) || !mxIsDouble(ovtxIdxs) || !mxIsDouble(fvtxIdxs))
    {
        std::cerr << "Error - struct.PoseLM.Vertices/Vi/Fi not stored as doubles!" << std::endl;
        return false;
    }   // end if

    const mwSize* mdims = mxGetDimensions( vtxs);
    const int numLMs = (int)mdims[1];
    if ( mdims[0] != 3)
    {
        std::cerr << "Error - struct.PoseLM.Vertices num rows != 3!" << std::endl;
        return false;
    }   // end if

    mdims = mxGetDimensions( ovtxIdxs);
    if ( mdims[1] != numLMs)
    {
        std::cerr << "Error - struct.PoseLM.Vi num cols != struct.PoseLM.Vertices num cols" << std::endl;
        return false;
    }   // end

    mdims = mxGetDimensions( fvtxIdxs);
    if ( mdims[1] != numLMs)
    {
        std::cerr << "Error - struct.PoseLM.Fi num cols != struct.PoseLM.Vertices num cols" << std::endl;
        return false;
    }   // end

    const double* vtx = (const double*)mxGetData(vtxs);
    const double *oidx = (const double*)mxGetData(ovtxIdxs);
    const double *fidx = (const double*)mxGetData(fvtxIdxs);
    const char** names = NULL;
    if ( lmnames != NULL)
        names = (const char**)mxGetData(lmnames);

    _lmcoords.resize(numLMs);
    _lmovis.resize(numLMs);
    _lmofis.resize(numLMs);
    _lmnames.resize(numLMs);
    for ( int i = 0; i < numLMs; ++i)
    {
        _lmcoords[i] = cv::Vec3f( vtx[0], vtx[1], vtx[2]);
        _lmovis[i] = _vtxIndices[(int)oidx[i]];
        _lmofis[i] = _faceIndices[(int)fidx[i]];
        if ( names != NULL)
            _lmnames[i] = names[i];
        else
            _lmnames[i] = "Unnamed";
        vtx += 3;
    }   // end for

    return true;
}   // end readLandmarks


bool MatlabReader::readInStructContents()
{
    if ( !_svar)
        return false;

    if ( !mxIsStruct(_svar))
        return false;

    mwSize nElems = (mwSize)mxGetNumberOfElements(_svar);
    if ( nElems != 1)
    {
        std::cerr << "Invalid number of elements (should be 1) found in mxArray!" << std::endl;
        return false;
    }   // end if

    const mwSize nFields = (mwSize)mxGetNumberOfFields(_svar);
    //std::cerr << "Found " << (int)nFields << " fields in struct" << std::endl;

    _objModel = RFeatures::ObjModel::create();  // Create a new object model

    if ( !readModelGeometry())
        return false;

    if ( !readTextureMap())
        return false;

    if ( !readBoundaryVertices())
        return false;

    if ( !readLandmarks())
        return false;

    _modelfile = FaceApp::IO::getStringValue( mxGetField( _svar, 0, "ModelFile"));
    mxArray* tsArr = mxGetField( _svar, 0, "CaptureTS");    // Timestamp (long)
    _capturets = (long)mxGetScalar( tsArr);

    return true;
}   // end readInStructContents



// Because the MAT file stores vertex indices from 1 (not 0), need to map these indices as
// the indices of _vtxIndices to the ObjModel stored vertex indices. Similarly with _faceIndices.
bool MatlabReader::readModelGeometry()
{
    mxArray* vtxArr = mxGetField( _svar, 0, "Vertices");
    mxArray* uvArr = mxGetField( _svar, 0, "UV");
    mxArray* facesArr = mxGetField( _svar, 0, "Faces");
    if ( !vtxArr || !uvArr || !facesArr)
    {
        std::cerr << "Didn't find all of 'Vertices', 'UV', or 'Faces' arrays in MAT file struct" << std::endl;
        return false;
    }   // end if

    if ( !mxIsDouble(vtxArr))
    {
        std::cerr << "Error - Vertices array is not stored as double precision!" << std::endl;
        return false;
    }   // end if

    if ( !mxIsDouble(uvArr))
    {
        std::cerr << "Error - UV array is not stored as double precision!" << std::endl;
        return false;
    }   // end if

    if ( !mxIsDouble(facesArr))
    {
        std::cerr << "Error - Faces array is not stored as double precision!" << std::endl;
        return false;
    }   // end if

    if ( mxGetNumberOfDimensions(vtxArr) != 2 || mxGetNumberOfDimensions(uvArr) != 2 || mxGetNumberOfDimensions(facesArr) != 2)
    {
        std::cerr << "Error - Vertices, UV and Faces arrays must be two dimensional!" << std::endl;
        return false;
    }   // end if

    const mwSize* mwDims = mxGetDimensions( vtxArr);
    if ( (int)mwDims[0] != 3)
    {
        std::cerr << "Error - Vertices array must have exactly three rows!" << std::endl;
        return false;
    }   // end if
    const int numVtxs = (int)mwDims[1];    // Number of vertices

    mwDims = mxGetDimensions( facesArr);
    if ( (int)mwDims[0] != 3)
    {
        std::cerr << "Error - Faces array must have three dimensions (vertices) per face!" << std::endl;
        return false;
    }   // end if
    const int numFaces = (int)mwDims[1];    // Number of faces

    mwDims = mxGetDimensions( uvArr);
    if ( (int)mwDims[0] != 2)
    {
        std::cerr << "Error - UV array must have exactly two rows!" << std::endl;
        return false;
    }   // end if

    if ( (int)mwDims[1] != numVtxs)
    {
        std::cerr << "Error - UV array number of columns does not match number of columns in array Vertices!" << std::endl;
        return false;
    }   // end if

    // Add all the vertices and texture offsets to the model.
    const double* vtx = (const double*)mxGetData( vtxArr);
    const double* uv = (const double*)mxGetData( uvArr);
    _vtxIndices.resize(numVtxs+1);
    for ( int i = 0; i < numVtxs; ++i)
    {
        const cv::Vec3f v( vtx[0], vtx[1], vtx[2]);
        const cv::Vec2f t( uv[0], 1.0 - uv[1]); // Invert the vertical offset
        const int objVtxId = _objModel->addVertex( v, t);
        if ( objVtxId == -1)
        {
            std::cerr << "Error - Unable to add vertex to object model!" << std::endl;
            return false;
        }   // end if
        vtx += 3;
        uv += 2;

        //_vtxIndices[i+1] = objVtxId; // Map the MAT vertex index (1 based) to the ObjModel vertex index (zero based)
        _vtxIndices[i+1] = i;
    }   // end for

    // Add the faces to the model
    const double* fv = (const double*)mxGetData( facesArr);
    int objFaceVtxIdxs[3];
    _faceIndices.resize(numFaces+1);
    for ( int i = 0; i < numFaces; ++i)
    {
        // Get the MAT file stored vertex indices, mapped to the required ObjModel indices
        objFaceVtxIdxs[0] = _vtxIndices[(int)fv[0]];
        objFaceVtxIdxs[1] = _vtxIndices[(int)fv[1]];
        objFaceVtxIdxs[2] = _vtxIndices[(int)fv[2]];

        const int objFaceId = _objModel->setFace( objFaceVtxIdxs);
        if ( objFaceId == -1)
        {
            std::cerr << "Error - Unable to add face to object model!" << std::endl;
            return false;
        }   // end if
        fv += 3;

        // Map the MAT face index (1 based) to the ObjModel face index (zero based)
        _faceIndices[i+1] = objFaceId;
    }   // end for

    return true;
}   // end readModelGeometry



bool MatlabFaceModelImport::load( const std::string& fname)
{
    _errMsg = "";
    bool loadedOkay = true;

    MATFile* mfile = matOpen( fname.c_str(), "r");
    if ( !mfile)
    {
        _errMsg = "Unable to open " + fname + " for reading!";
        loadedOkay = false;
    }   // end if

    mxArray* structVar = NULL;
    if ( loadedOkay)
    {
        structVar = matGetVariable( mfile, "struct");
        if ( !structVar)
        {
            _errMsg = "Unable to obtain 'struct' variable from MAT file " + fname;
            loadedOkay = false;
        }   // end if
    }   // end if

    if ( loadedOkay)
    {
        MatlabReader matReader( structVar);
        if ( !matReader.readInStructContents())
        {
            _errMsg = "Invalid data read in from MAT file " + fname;
            loadedOkay = false;
        }   // end if
        else
            _fmodel = createFaceModel( matReader);
    }   // end if

    if ( mfile && matClose( mfile) != 0)
        std::cerr << "Error on closure of file " << fname << std::endl;

    return loadedOkay;
}   // end load


// protected
bool MatlabFaceModelImport::read( std::istream& is)
{
    // Matlab does not currently provide a C++ streams style API so can't use this interface!
    throw FaceApp::IO::IOException(
            "MatlabFaceModelImport::read is not supported. Use MatlabFaceModelImport::load instead.");
    return false;   // Req for compiler warning
}   // end read

