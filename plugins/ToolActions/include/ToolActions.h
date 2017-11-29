#ifndef FACE_APP_STATIC_PLUGIN_TOOL_ACTIONS_H
#define FACE_APP_STATIC_PLUGIN_TOOL_ACTIONS_H

#define QT_STATICPLUGIN
#include <FaceActionInterface.h>   // FaceTools

namespace FaceApp {

class ToolActions : public FaceTools::FaceActionGroup
{
Q_OBJECT
Q_PLUGIN_METADATA( IID FaceToolsPluginFaceActionInterface_iid)
Q_INTERFACES( FaceTools::FaceActionInterface)
public:
    ToolActions();
    virtual ~ToolActions(){}

    void initFaceDetection( const QString& haarModelsDir, const QString& faceLmksShapeModel);

    virtual void addTo( QMenu*) const;
    virtual void addTo( QToolBar*) const;
    virtual void addToContext( QMenu*) const;

private:
    std::vector<FaceTools::FaceAction*> _actions;
};  // end class

}   // end namespace

#endif


