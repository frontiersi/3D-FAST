#ifndef FACE_APP_STATIC_PLUGIN_SURFACE_VISUALISATION_H
#define FACE_APP_STATIC_PLUGIN_SURFACE_VISUALISATION_H

#include <VisualisationAction.h>

namespace FaceApp {

class SurfaceVisualisation : public FaceTools::VisualisationAction
{ Q_OBJECT
public:
    explicit SurfaceVisualisation( const std::string& iconfile="");

    const QIcon* getIcon() const override { return &_icon;}
    const QKeySequence* getShortcut() const override { return &_keys;}
    QString getDisplayName() const override { return "Surface";}

    bool isAvailable( const FaceTools::FaceControl*) const override { return true;}
    bool allowSetColour() const override { return true;}
    void mapActor( FaceTools::FaceControl*) override;

protected:
    bool isDefault( const FaceTools::FaceControl*) const override;

private:
    const QIcon _icon;
    const QKeySequence _keys;
};  // end class

}   // end namespace

#endif


