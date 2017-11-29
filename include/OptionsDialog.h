#ifndef FACE_APP_OPTIONS_DIALOG_H
#define FACE_APP_OPTIONS_DIALOG_H

#include <FaceView.h>   // FaceTools
#include <QDialog>
#include <QAbstractButton>
#include <QColor>
#include <string>

namespace Ui { class OptionsDialog; }
namespace FaceApp {

class OptionsDialog : public QDialog
{ Q_OBJECT
public:
    explicit OptionsDialog( QWidget *parent=0);
    ~OptionsDialog() override;

    // Set the range (minv, maxv), and the current values (v0,v1)
    // and with the number of colours to map (ncols >= 2).
    void setMinMaxLookup( float minv, float maxv, float v0, float v1, size_t ncols);
    void setMinMidMaxColours( const QColor& c0, const QColor& c1, const QColor& c2);

    float getLandmarkBaseRadius();
    const QColor& getLandmarkBaseColour();

    double getFaceCropFactor();

    int getModelLineWidth();
    int getModelVertexSize();
    const QColor& getModelFlatSurfaceColour();

    // Legend colour mapping for model surface
    float getLegendMinLookupVal();
    float getLegendMaxLookupVal();
    const QColor& getLegendMinColour();
    const QColor& getLegendMaxColour();
    int getLegendNumLookupColours();
    double getOpacity() const;

    std::string getMeasurementUnits();
    const QColor& getTextColour();
    bool getShowCaptions();
    bool getShowAxes();
    bool getBackfaceCulling();

    // Pass in load/save filename
    bool load( const std::string&);
    bool save( const std::string&) const;

signals:
    void onOptionsChanged();

private slots:
    void doClickedSetLandmarkBaseColour();

    void doClickedSetModelFlatSurfaceColour();
    void doClickedSetLegendMinColour();
    void doClickedSetLegendMaxColour();
    void doClickedSetTextColour();

    void clickedButtonBox( QAbstractButton* button);

private:
    Ui::OptionsDialog *ui;

    float _acceptedLandmarkBaseRadius;
    QColor _acceptedLandmarkBaseColour;

    double _acceptedFaceCropFactor;

    int _acceptedModelLineWidth;
    int _acceptedModelVertexSize;
    float _acceptedLegendMinLookupVal;
    float _acceptedLegendMaxLookupVal;
    QColor _acceptedModelFlatSurfaceColour;
    QColor _acceptedLegendMinColour;
    QColor _acceptedLegendMaxColour;
    int _acceptedLegendNumLookupColours;
    bool _acceptedBackfaceCulling;
    bool _acceptedShowCaptions;
    bool _acceptedShowAxes;
    bool _acceptedCleanOnLoad;
    std::string _acceptedMeasurementUnits;
    QColor _acceptedTextColour;

    QColor _curLandmarkBaseColour;
    QColor _curModelFlatSurfaceColour;
    QColor _curLegendMinColour;
    QColor _curLegendMaxColour;
    QColor _curTextColour;

    void acceptCurrentValues();
    void restoreAcceptedValues();   // Also emits optionsChanged

    OptionsDialog( const OptionsDialog&);
    void operator=( const OptionsDialog&);
};  // end class

}   // end namespace

#endif // OPTIONSDIALOG_H
