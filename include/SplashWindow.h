#ifndef FACE_APP_SPLASH_WINDOW_H
#define FACE_APP_SPLASH_WINDOW_H

#include <QSplashScreen>

namespace Ui { class SplashWindow;}

namespace FaceApp {

class SplashWindow : public QSplashScreen
{ Q_OBJECT
public:
    SplashWindow();
    ~SplashWindow() override;

    QSize sizeHint() const override { return QSize(600,130);}

signals:
    void clickedLink(bool);

private:
    Ui::SplashWindow *ui;
};  // end class

}   // end namespace

#endif
