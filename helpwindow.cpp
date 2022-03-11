#include "helpwindow.h"
#include "ui_helpWindow.h"

HelpWindow::HelpWindow(QWidget *parent) : QWidget(parent), ui(new Ui::helpWindow)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::Window);
    this->close();
}

HelpWindow::~HelpWindow()
{

}
