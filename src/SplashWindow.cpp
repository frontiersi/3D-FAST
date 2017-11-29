#include <SplashWindow.h>
#include <ui_SplashWindow.h>
#include <QStyle>
#include <QDesktopWidget>
#include <QApplication>
using FaceApp::SplashWindow;


SplashWindow::SplashWindow()
    : QSplashScreen(), ui( new Ui::SplashWindow)
{
    ui->setupUi(this);
    connect( ui->licenseButton, &QPushButton::clicked, [=](){ hide(); emit clickedLink(true);});
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    QPixmap pixmap( ":/logos/splash.png");
    QPalette palette;
    palette.setBrush( backgroundRole(), QBrush( pixmap));
    setPalette(palette);
    setFixedSize(pixmap.size());
    setWindowOpacity(0.7);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
}   // end ctor


SplashWindow::~SplashWindow()
{
    delete ui;
}   // end dtor
