#ifndef FACE_APP_STATIC_PLUGIN_TEXTURE_VISUALISATION_H
#define FACE_APP_STATIC_PLUGIN_TEXTURE_VISUALISATION_H

#include <VisualisationAction.h>

namespace FaceApp {

class TextureVisualisation : public FaceTools::VisualisationAction
{ Q_OBJECT
public:
    explicit TextureVisualisation( const std::string& iconfile="");

    const QIcon* getIcon() const override { return &_icon;}
    const QKeySequence* getShortcut() const override { return &_keys;}
    QString getDisplayName() const override { return "Texture";}

    bool isAvailable( const FaceTools::FaceControl*) const override;
    bool useTexture() const override { return true;}
    void mapActor( FaceTools::FaceControl*) override {}

protected:
    bool isDefault( const FaceTools::FaceControl*) const override;

private:
    const QIcon _icon;
    const QKeySequence _keys;
};  // end class

}   // end namespace

#endif
