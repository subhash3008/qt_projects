#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>

///////////////////////////////////
// Global Variables
//////////////////////////////////
QMainWindow *main_window;
QListWidget *notes_list;
QTextEdit *notes_editor;
QPushButton *add_note_btn;
QPushButton *save_btn;
QPushButton *delete_btn;
QMap<QString, QString> notes;

///////////////////////////////////
// Local function declaration
//////////////////////////////////
void setupUI();
void saveCurrentNote();
void deleteSelectedNote();
void onNoteSelection(QListWidgetItem *selectedItem);
void loadNotesPersistent();
void saveNotesPersistent();
void updateNotesList();

///////////////////////////////////
// main
//////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    setupUI();

    return app.exec();
}

///////////////////////////////////
// Local function definitions
//////////////////////////////////
void setupUI()
{
    main_window = new QMainWindow;
    main_window->setWindowTitle("Note It");
    main_window->resize(800, 600);

    QWidget *central_widget = new QWidget;
    QHBoxLayout *main_layout = new QHBoxLayout(central_widget);

    main_window->setCentralWidget(central_widget);

    // Left panel
    QWidget *left_panel_widget = new QWidget;
    QVBoxLayout *left_panel_layout = new QVBoxLayout(left_panel_widget);
    notes_list = new QListWidget;
    left_panel_layout->addWidget(notes_list);

    delete_btn = new QPushButton("Delete note");
    left_panel_layout->addWidget(delete_btn);

    add_note_btn = new QPushButton("Add new note");
    left_panel_layout->addWidget(add_note_btn);

    main_layout->addWidget(left_panel_widget, 1);

    // Right Panel
    QWidget *right_panel_widget = new QWidget;
    QVBoxLayout *right_panel_layout = new QVBoxLayout(right_panel_widget);
    notes_editor = new QTextEdit;
    notes_editor->setPlaceholderText("Write your notes here...");
    right_panel_layout->addWidget(notes_editor);

    save_btn = new QPushButton("Save Note");
    right_panel_layout->addWidget(save_btn);
    main_layout->addWidget(right_panel_widget, 2);


    // Event connections
    QObject::connect(add_note_btn, &QPushButton::clicked, [&]() {
        notes_editor->clear();
        notes_list->clearSelection();
    });
    QObject::connect(save_btn, &QPushButton::clicked, saveCurrentNote);
    QObject::connect(delete_btn, &QPushButton::clicked, deleteSelectedNote);
    QObject::connect(notes_list, &QListWidget::itemClicked, onNoteSelection);
    QObject::connect(main_window, &QMainWindow::destroyed, saveNotesPersistent);

    // Load notes from saved file
    loadNotesPersistent();
    updateNotesList();

    main_window->show();
}

void saveCurrentNote()
{
    QString note_title = notes_editor->toPlainText().left(30).trimmed();
    if (note_title.isEmpty())
    {
        QMessageBox::warning(main_window, "Error", "Note title cannot be empty");
    }
    notes[note_title] = notes_editor->toPlainText();
    saveNotesPersistent();
    updateNotesList();
}

void deleteSelectedNote()
{

    QListWidgetItem *selectedItem = notes_list->currentItem();
    if(!selectedItem)
    {
        QMessageBox::warning(main_window, "Error", "No notes selected for delete operation.");
        return;
    }

    QString title = selectedItem->text();
    // notes_list->removeItemWidget(selectedItem);
    notes.remove(title);
    delete notes_list->takeItem(notes_list->row(selectedItem));
    saveNotesPersistent();
    updateNotesList();
    notes_editor->clear();
}

void onNoteSelection(QListWidgetItem *selectedItem)
{
    QString title = selectedItem->text();
    notes_editor->setText(notes[title]);
}


void loadNotesPersistent()
{
    QFile file("notes.dat");
    if(file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        in >> notes;
        file.close();
    }
    updateNotesList();
}

void saveNotesPersistent()
{
    QFile file("notes.dat");
    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out << notes;
        file.close();
    }
}

void updateNotesList()
{
    notes_editor->clear();
    notes_list->clear();

    for(auto &title: notes.keys())
    {
        notes_list->addItem(title);
    }
}
