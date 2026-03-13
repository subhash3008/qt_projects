#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QMap>
#include <QPair>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QCryptographicHash> // for password hashing
#include <QFile>
#include <QDataStream>

QMap<QString, QPair<QString, QString>> passwords;

QMainWindow *main_window;
QTableWidget *pass_table;

QPushButton *add_btn;
QPushButton *copy_btn;
QPushButton *remove_btn;

bool verifyMasterPassword();
void addPassword();
void copyPassword();
void removePassword();
void savePasswords();
void loadPasswords();

void setUpUI();

int main(int argc, char *argv[])
{
    QApplication pass_manager(argc, argv);
    setUpUI();

    if (verifyMasterPassword())
    {
        loadPasswords();
        main_window->show();
    }

    return pass_manager.exec();
}

void setUpUI()
{
    main_window = new QMainWindow;
    main_window->setWindowTitle("Password Manager");
    main_window->setMinimumSize(600, 400);

    QWidget *central_widget = new QWidget;
    QVBoxLayout *main_layout = new QVBoxLayout(central_widget);

    pass_table = new QTableWidget(0, 3);
    pass_table->setHorizontalHeaderLabels({"Service", "Username", "Password"});
    pass_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pass_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pass_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    main_layout->addWidget(pass_table);

    QHBoxLayout *btn_layout = new QHBoxLayout;
    add_btn = new QPushButton("Add Password");
    copy_btn = new QPushButton("Copy Password");
    remove_btn = new QPushButton("Remove Password");

    QObject::connect(add_btn, &QPushButton::clicked, addPassword);
    QObject::connect(copy_btn, &QPushButton::clicked, copyPassword);
    QObject::connect(remove_btn, &QPushButton::clicked, removePassword);

    btn_layout->addWidget(add_btn);
    btn_layout->addWidget(copy_btn);
    btn_layout->addWidget(remove_btn);

    main_layout->addLayout(btn_layout);

    main_window->setCentralWidget(central_widget);

    // save all passwords before closing
    QObject::connect(QApplication::instance(), &QCoreApplication::aboutToQuit, savePasswords);

}


void addPassword()
{
    QString service = QInputDialog::getText(nullptr, "Add password", "Service");

    if (service.isEmpty()) return;

    QString username = QInputDialog::getText(nullptr, "Add password", "Username");
    QString password = QInputDialog::getText(nullptr, "Add password", "Password");

    passwords[service] = qMakePair(username, password);

    int row = pass_table->rowCount();
    pass_table->insertRow(row);
    pass_table->setItem(row, 0, new QTableWidgetItem(service));
    pass_table->setItem(row, 1, new QTableWidgetItem(username));
    pass_table->setItem(row, 2, new QTableWidgetItem("********"));
}

void copyPassword()
{
    int row = pass_table->currentRow();

    if (row >= 0)
    {
        QString service = pass_table->item(row, 0)->text();
        QApplication::clipboard()->setText(passwords[service].second);
        QMessageBox::information(nullptr, "Copied", "Password is copied to clipboard");
    }
}

void removePassword()
{
    int row = pass_table->currentRow();

    if (row >= 0)
    {
        QString service = pass_table->item(row, 0)->text();
        passwords.remove(service);
        pass_table->removeRow(row);
    }
}

bool verifyMasterPassword()
{
    QString input = QInputDialog::getText(nullptr, "Master Password", "Enter master password", QLineEdit::Password);
    if (input.isEmpty())
    {
        QMessageBox::warning(nullptr, "Master Password", "Master password cannot be empty");
        return false;
    }

    QString stored_hash = QCryptographicHash::hash(
        QString("12345").toUtf8(),
        QCryptographicHash::Sha256
        ).toHex();

    QString input_hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256).toHex();

    bool is_pass_correct = input_hash == stored_hash;

    if (!is_pass_correct)
    {
        QMessageBox::warning(nullptr, "Master Password", "Wrong password!!");
    }
    return is_pass_correct;
}

void savePasswords()
{
    //data = GMAIL | user1@gmail.com | 12345
    QString data;
    for (auto it = passwords.begin(); it != passwords.end(); ++it)
    {
        data += it.key() + "|" + it.value().first + "|" + it.value().second + "\n";
    }
    QFile file("password.dat");
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out << data;
        file.close();
    }
}

void loadPasswords()
{
    QFile file("password.dat");
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        QString data;
        in >> data;

        QStringList entries = data.split("\n");
        for (const QString &entry: entries)
        {
            QStringList pass_params = entry.split("|");
            if (pass_params.size() == 3)
            {
                passwords[pass_params[0]] = qMakePair(pass_params[1], pass_params[2]);

                int row = pass_table->rowCount();
                pass_table->insertRow(row);
                pass_table->setItem(row, 0, new QTableWidgetItem(pass_params[0]));
                pass_table->setItem(row, 1, new QTableWidgetItem(pass_params[1]));
                pass_table->setItem(row, 2, new QTableWidgetItem("********"));
            }
        }
        file.close();
    }
}
