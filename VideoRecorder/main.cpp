#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QFileDialog>

//////////////////////////////////////////////
/// GLOBAL VARIABLES
//////////////////////////////////////////////

const QString ffmpeg = "C:/Users/s.chandra/Downloads/ffmpeg-8.1-full_build/ffmpeg-8.1-full_build/bin/ffmpeg.exe";

//////////////////////////////////////////////
/// HELPER FUNCTIONS
//////////////////////////////////////////////

void startRecording(QProcess &record_process, QPushButton *record_btn, QString &output_file_path, QWidget *main_window);
void stopRecording(QProcess &record_process, QPushButton *record_btn, QString &output_file_path, QWidget *main_window);


//////////////////////////////////////////////
/// MAIN FUNCTION
//////////////////////////////////////////////
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget main_window;
    main_window.setWindowTitle("Screen Recorder");
    main_window.resize(300, 100);

    QVBoxLayout *layout = new QVBoxLayout(&main_window);

    QLabel *title_label = new QLabel("Screen Recorder");
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("font-size: 24px; color: blue;");
    layout->addWidget(title_label);

    QPushButton *record_btn = new QPushButton("Record");
    record_btn->setStyleSheet("font-size: 16px; color: white; background-color: green;");
    layout->addWidget(record_btn);


    QProcess record_process;
    QString output_file_path;

    QObject::connect(record_btn, &QPushButton::clicked, [&](){
        // check for correct button click
        if (record_btn->text() == "Record")
        {
            // get the path where the recording would be saved
            output_file_path = QFileDialog::getSaveFileName(&main_window, "Save recording", "", "Video Files (*.mp4, *.wav)");
            if (!output_file_path.isEmpty()) // check for valid output path
            {
                // Start recording
                startRecording(record_process, record_btn, output_file_path, &main_window);
            }
        }
        else
        {
            stopRecording(record_process, record_btn, output_file_path, &main_window);
        }
    });

    main_window.show();
    app.exec();

    return EXIT_SUCCESS;
}

/**
 * @brief startRecording    Function to start the recording of the video
 * @param record_process    Handler for video recording process
 * @param record_btn        Button for starting or stopping the video recording
 * @param output_file_path  Location where the recording file would be stored
 * @param main_window       Main application window
 */
void startRecording(QProcess &record_process, QPushButton *record_btn, QString &output_file_path, QWidget *main_window)
{
    // Update UI of record button
    record_btn->setText("Stop recording");
    record_btn->setStyleSheet("font-style: 16px; color: white; background-color: red;");

    QString command = ffmpeg;

    // create arguments list for ffmpeg
    QStringList arguments;
    arguments << "-y" // overwrite output file without asking
              << "-f" << "gdigrab"  // uses GDI GRAB captures the screen
              << "-i" << "desktop"  // capture the entire screen
              << "-f" << "dshow"    // use direct show for audio capture
              << "-i" << "audio=Mikrofonarray (Realtek(R) Audio)"  // Audio device
              << "-c:v" << "libx264" // encodes video with x264
              << "-preset" << "ultrafast" // encoding preset
              << "-c:a" << "aac"    // encoding audio with AAC
              << "-pix_fmt" << "yuv420p"    // pixel format for compatibility
              << output_file_path;  // output file

    record_process.start(command, arguments); // trigger the video recording to be started
    if (!record_process.waitForStarted())   // check if video recording has started
    {
        qDebug() << "Failed to start recording.\n";

        // Reset the UI to be able to start the recording again
        record_btn->setText("Record");
        record_btn->setStyleSheet("font-style: 16px; color: white; background-color: green;");
    }
    else
    {
        qDebug() << "Recording started.\n";
        main_window->showMinimized();
    }
}

/**
 * @brief stopRecording     Function to start the recording of the video
 * @param record_process    Handler for video recording process
 * @param record_btn        Button for starting or stopping the video recording
 * @param output_file_path  Location where the recording file would be stored
 * @param main_window       Main application window
 */
void stopRecording(QProcess &record_process, QPushButton *record_btn, QString &output_file_path, QWidget *main_window)
{
    if (record_process.state() == QProcess::Running) // Check if the recording is in progress
    {
        // Stop the recording and wait for it to stop
        record_process.write("q\n");
        record_process.waitForFinished();

        qDebug() << "Recording stopped. Saved to " << output_file_path;

        // Update UI after recording has stopped
        record_btn->setText("Record");
        record_btn->setStyleSheet("font-style: 16px; color: white; background-color: green;");
        main_window->showNormal();
    }
}

