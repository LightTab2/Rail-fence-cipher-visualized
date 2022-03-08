#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>

#define STEP_TIME       160

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    enum validateState
    {
        Invalid,
        Valid,
        Special
    };

    void            execute         ();
    void            executing       ();
    void            stopExecuting   ();

    void            cypherStep      ();
    void            decypherStep    ();

    void            setupGuiLabels  (int state);

    void            saveFile        ();
    void            saveAsFile      ();
    void            loadFile        ();

    unsigned        iteration       = 0,
                    visIteration    = 0,
                    maxIterations   = 0;

    QString         text            = "",
                    processedText   = "",
                    savedFileName   = "";

    bool            cypherMode      = false; //false - szyfrowanie, true - deszyfrowanie
    unsigned        *posArray       = nullptr;
    QTimer          executeTimer;
};
