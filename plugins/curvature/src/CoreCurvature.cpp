#include <CoreCurvature.h>
#include <MinCurvature.h>
//#include <FirstOrderMinCurvature.h>
//#include <SecondOrderMinCurvature.h>
#include <MaxCurvature.h>
//#include <FirstOrderMaxCurvature.h>
//#include <SecondOrderMaxCurvature.h>
#include <MeanCurvature.h>
#include <GaussianCurvature.h>
#include <NormalDeterminant.h>
#include <AbsCurvature.h>
using FaceApp::Visualisation::CoreCurvature;

void initResources()
{
    Q_INIT_RESOURCE(icons);
}   // end initResources

CoreCurvature::CoreCurvature() : FaceTools::FaceActionGroup()
{
    initResources();
    addAction( new FaceApp::Visualisation::MaxCurvature());
    //addAction( new FaceApp::Visualisation::FirstOrderMaxCurvature());
    //addAction( new FaceApp::Visualisation::SecondOrderMaxCurvature());

    addAction( new FaceApp::Visualisation::MinCurvature());
    //addAction( new FaceApp::Visualisation::FirstOrderMinCurvature());
    //addAction( new FaceApp::Visualisation::SecondOrderMinCurvature());

    addAction( new FaceApp::Visualisation::AbsCurvature());
    addAction( new FaceApp::Visualisation::MeanCurvature());
    addAction( new FaceApp::Visualisation::GaussianCurvature());
    addAction( new FaceApp::Visualisation::NormalDeterminant());
}   // end ctor
