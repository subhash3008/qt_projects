#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QMediaDevices>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QCheckBox>
#include <QPushButton>
#include <QImageCapture>
#include <QFileDialog>

QImage toGrayScale(const QImage& img);

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QMainWindow *window = new QMainWindow;
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QCameraDevice cameraDevice = QMediaDevices::defaultVideoInput();
    if (cameraDevice.isNull())
    {
        layout->addWidget(new QLabel("No Camera found."));
        window->setCentralWidget(centralWidget);
        window->show();
        return app.exec();
    }

    QLabel *videoLabel = new QLabel("Camera Feed");
    videoLabel->setMinimumSize(640, 480);
    videoLabel->setStyleSheet("background-color: black");
    videoLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(videoLabel);

    QCheckBox *bwCheckBox = new QCheckBox("Black and White filter");
    layout->addWidget(bwCheckBox);
    QPushButton *captureBtn = new QPushButton("Take Picture");
    layout->addWidget(captureBtn);

    QCamera *camera = new QCamera(cameraDevice);
    QMediaCaptureSession *session = new QMediaCaptureSession();
    session->setCamera(camera);
    QVideoSink *videoSink = new QVideoSink();
    session->setVideoSink(videoSink);

    QImageCapture *imgCapture = new QImageCapture();
    session->setImageCapture(imgCapture);

    QImage currentFrame;
    QObject::connect(videoSink, &QVideoSink::videoFrameChanged, [&](const QVideoFrame &frame) {
        if (!frame.isValid()) return;

        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QVideoFrame::ReadOnly);
        QImage image = cloneFrame.toImage();
        cloneFrame.unmap();

        currentFrame = bwCheckBox->isChecked() ? toGrayScale(image) : image;
        videoLabel->setPixmap(QPixmap::fromImage(currentFrame).scaled(videoLabel->size(), Qt::KeepAspectRatio, Qt::FastTransformation));

    });

    QObject::connect(captureBtn, &QPushButton::clicked, [&]() {
        if (currentFrame.isNull()) return;

        QString path = QFileDialog::getSaveFileName(nullptr, "Save Image", "Images (*.png, *.jpg)");

        if (!path.isEmpty())
        {
            currentFrame.save(path);
        }
    });

    camera->start();

    window->setCentralWidget(centralWidget);
    window->setWindowTitle("Camera App");
    window->resize(700, 600);
    window->show();

    return app.exec();
}


QImage toGrayScale(const QImage& img)
{
    return img.convertToFormat(QImage::Format_Grayscale16);
}
