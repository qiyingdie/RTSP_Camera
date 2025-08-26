#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->connectButton->setEnabled(false);

    init_SSH();
    init_Rtsp();

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::play_Event);



}

MainWindow::~MainWindow()
{
    delete ui;
    closure_Play();
    closure_SSH();
}

void MainWindow::play_Event()
{
    if (paly_flag) {
        closure_Play();
        ui->connectButton->setText("播放");
    } else {
        start_Play();
        ui->connectButton->setText("停止");
    }

}

void MainWindow::FPS_Detect(const QVideoFrame &frame)
{
    if (!frame.isValid()) return;

    // 第一次开始计时
    if (!timer.isValid()) {
        timer.start();
        frame_Count = 0;
    }

    frame_Count++;

    if (timer.elapsed() > 1000) { // 每秒刷新一次
        double fps = frame_Count * 1000.0 / timer.elapsed();
        ui->fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 2));

        frame_Count = 0;
        timer.restart();
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    ui->statusLabel->setText("就绪");
    ui->connectButton->setEnabled(true);
    killTimer(timerId);
}

//Rtsp初始化
void MainWindow::init_Rtsp()
{
    player = new QMediaPlayer(this);
    videoWidget = new QVideoWidget(this);

    sink = videoWidget->videoSink();


    connect(sink, &QVideoSink::videoFrameChanged, this, &MainWindow::FPS_Detect);

    timerId = startTimer(6000);

    ui->verticalLayout_3->addWidget(videoWidget);
    player->setVideoOutput(videoWidget);


}

//开始播放
void MainWindow::start_Play()
{

    player->setSource(QUrl("rtsp address"));
    player->play();

    paly_flag = true;
}

//关闭播放
void MainWindow::closure_Play()
{

    player->stop();
    player->setSource(QUrl());

    paly_flag = false;
}

//SSH连接初始化
void MainWindow::init_SSH()
{
    process = new QProcess(this);
    QString host = "ip address";

    QString sshPath = getLocal_SSHAddress();
    QString privateKeyPath = getLocal_PrivateKeyPath();
    if (!QFile::exists(privateKeyPath)) {
        qDebug() << "SSH private key not found at:" << privateKeyPath;
        return;
    }

    QObject::connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
        qDebug() << "Output:" << QString::fromUtf8(process->readAllStandardOutput());
    });
    QObject::connect(process, &QProcess::readyReadStandardError, this, [this]() {
        qDebug() << "Error:" << QString::fromUtf8(process->readAllStandardError());
    });
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     this, [this](int exitCode, QProcess::ExitStatus status) {
                         qDebug() << "Command finished with code:" << exitCode;
                         if (exitCode != 0) {
                             qDebug() << "Possible reasons:"
                                      << "\n- SSH key not authorized"
                                      << "\n- Key file permission issues"
                                      << "\n- Incorrect path to key file";
                         }
                         closure_SSH();
                     }
                     );

    QStringList arguments;
    arguments << "-i" << privateKeyPath
              << "-o" << "StrictHostKeyChecking=no"
              << "-o" << "ServerAliveInterval=60"
              << host;

    process->start(sshPath, arguments);

    sending_Commands(". /home/qiyingdie/Documents/Mediamtx/rstp_Start.sh");

    //超时返回
    if (!process->waitForStarted(5000)) {
        qDebug() << "Failed to start SSH process:" << process->errorString();
        closure_SSH();
    }
}

//关闭SSH连接
void MainWindow::closure_SSH()
{
    if (process && process->state() == QProcess::Running) {
        // 发送退出命令
        sending_Commands(". /home/qiyingdie/Documents/Mediamtx/rstp_Stop.sh");
        process->write("exit\n");
        process->waitForFinished(2000);
    }

    if (process) {
        process->kill();
        process->deleteLater();
        process = nullptr;
    }
}

//发送命令和接收返回值
void MainWindow::sending_Commands(const QString& command)
{
    if (!process || process->state() != QProcess::Running) {
        qDebug() << "SSH connection not established";
        return;
    }

    // 发送命令（添加换行符模拟回车）
    process->write((command + "\n").toUtf8());
    process->waitForBytesWritten(2000);

    //读取返回值
    // QByteArray output = process->readAllStandardOutput();
}

//获取本地私钥
QString MainWindow::getLocal_PrivateKeyPath()
{
    #ifdef Q_OS_WIN
        // Windows: 查找用户目录下的.ssh目录
        wchar_t profilePath[MAX_PATH];
        if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, profilePath) == S_OK) {
            QString homeDir = QString::fromWCharArray(profilePath);
            return homeDir + "/.ssh/id_BaiYu_rsa";
        }
        return "C:/Users/qiyingdie/.ssh/id_BaiYu_rsa"; // 备用位置
    #else
        // Linux/macOS
        return QDir::homePath() + "/.ssh/id_BaiYu_rsa";
    #endif
}

//获取本地SSH地址
QString MainWindow::getLocal_SSHAddress()
{
    #ifdef Q_OS_WIN
        // Windows系统查找ssh.exe
        QStringList possiblePaths = {
            "C:/Windows/System32/OpenSSH/ssh.exe"
        };

        for (const QString &path : possiblePaths) {
            if (QFile::exists(path)) {
                return path;
            }
        }

        // 最后尝试PATH环境变量
        return "ssh.exe";
    #else
        // Linux/macOS
        return "ssh";
    #endif
}




