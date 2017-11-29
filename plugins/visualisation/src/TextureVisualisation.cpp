#include <TextureVisualisation.h>
#include <FaceControl.h>   // FaceTools
using FaceApp::TextureVisualisation;
using FaceTools::FaceControl;


TextureVisualisation::TextureVisualisation( const std::string& iconfile)
    : _icon(iconfile.c_str()), _keys( Qt::Key_4)
{
    init(); // FaceTools::FaceAction
}   // end ctor


// public
bool TextureVisualisation::isAvailable( const FaceControl* fcont) const
{
    return fcont->getModel()->getObjectMeta()->getObject()->getNumMaterials() > 0;
}   // end isAvailable


// protected
bool TextureVisualisation::isDefault( const FaceControl* fcont) const
{
    return isAvailable( fcont);
}   // end isDefault
