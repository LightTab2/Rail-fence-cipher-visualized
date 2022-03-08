#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Łączy naciśnięcie przycisku z funkcją szyfrującą lub deszyfrującą tekst (cypher.cpp)
    //https://doc.qt.io/qt-5/signalsandslots.html
    connect(ui->executeButton,  &QPushButton::clicked,      this, &MainWindow::execute);

    //Co ile milisekund będzie wykonywała się wizualizacja
    //https://doc.qt.io/qt-5/qtimer.html
    executeTimer.setInterval(STEP_TIME);

    //Timer będzie powtarzać funkcję [executing]
    executeTimer.callOnTimeout(this, &MainWindow::executing);
    executeTimer.setSingleShot(false);

    //Zmienia tekst podpisów
    connect(ui->modeCheck,      &QCheckBox::stateChanged,   this, &MainWindow::setupGuiLabels);

    //Zarządzanie plikami
    ui->actionLoad  ->setShortcut(QKeySequence(tr("Ctrl+O")));
    ui->actionSave  ->setShortcut(QKeySequence(tr("Ctrl+S")));
    ui->actionSaveAs->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));

    connect(ui->actionLoad,     &QAction::triggered,        this, &MainWindow::loadFile);
    connect(ui->actionSave,     &QAction::triggered,        this, &MainWindow::saveFile);
    connect(ui->actionSaveAs,   &QAction::triggered,        this, &MainWindow::saveAsFile);

    //Wyświetlanie pomocy
    connect(ui->actionInfo,     &QAction::triggered,        this, [this](){QMessageBox::information(this, tr("Informacja o algorytmie"),
                                                                                         tr("Szyfr Cezara ROT3. Dozwolony alfabet to [A-Z]. Znaki specjalne [ \t\n] są przepisywane. Reszta przepisywana jako \"?\"."));});}

//Odpowiada za zmiany napisów na formatkach
void MainWindow::setupGuiLabels(int state)
{
    cypherMode = (state == 2);

    if (cypherMode) //deszyfrowanie
    {
        ui->executeButton   ->setText("Deszyfruj");
        ui->firstTextLabel  ->setText("Szyfr");
        ui->secondTextLabel ->setText("Tekst");
    }
    else            //szyfrowanie
    {
        ui->executeButton   ->setText("Szyfruj");
        ui->firstTextLabel  ->setText("Tekst");
        ui->secondTextLabel ->setText("Szyfr");
    }
}

void MainWindow::saveFile()
{
    if (savedFileName == "")
        return;

    QFile file(savedFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Nie udało się otworzyć pliku"),
            file.errorString());

        return;
    }

    QString str = ui->secondText->toPlainText();
    QDataStream out(&file);
    out << str;
}

void MainWindow::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                 tr("Zapisz wygenerowany szyfr jako"), "",
                                                 tr("Pliki tekstowe (*.txt);;Wszystkie pliki (*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Nie udało się otworzyć pliku"),
            file.errorString());

        return;
    }

    QString str = ui->secondText->toPlainText();
    QDataStream out(&file);
    out << str;

    savedFileName = fileName;
}

void MainWindow::loadFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                 tr("Wczytaj szyfr"), "",
                                                 tr("Pliki tekstowe (*.txt);;Wszystkie pliki (*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Nie udało się otworzyć pliku"),
            file.errorString());

        return;
    }

    QString str;
    QDataStream in(&file);
    in >> str;

    ui->firstText->setPlainText(str);

    savedFileName = fileName;
}

MainWindow::~MainWindow()
{
    delete ui;
}
