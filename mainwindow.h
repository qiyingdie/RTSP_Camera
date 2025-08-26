#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//SSH需要的头文件
#include <QProcess>
#include <QFileDialog>
#include <QStyle>
//rtsp需要的头文件
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVideoSink>
#include <QVideoFrame>
//计时器
#include <QTimerEvent>
#include <QElapsedTimer>

#include <QPixmap>

//windows系统的特殊头文件
#ifdef Q_OS_WIN
    #include <windows.h> // 用于获取用户目录
    #include <shlobj.h>
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void play_Event();
    // void frame_Rate();
    void FPS_Detect(const QVideoFrame &frame);

private:
    Ui::MainWindow *ui;

    //ssh
    QProcess *process = nullptr;
    //rtsp
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QVideoSink *sink;

    bool paly_flag = false;
    //计时器
    int timerId;
    QElapsedTimer timer;
    int frame_Count = 0;

    //rtsp
    void init_Rtsp();
    void start_Play();
    void closure_Play();

    //ssh
    QString getLocal_PrivateKeyPath();
    QString getLocal_SSHAddress();
    void init_SSH();
    void closure_SSH();
    void sending_Commands(const QString& command);

    //计时器
    void timerEvent(QTimerEvent *event) override;

};
#endif // MAINWINDOW_H
