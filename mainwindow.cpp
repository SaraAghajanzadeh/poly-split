#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camerawindow.h"
#include <QLayout>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    renderArea = new RenderArea;
    ui->centralWidget->layout()->addWidget(renderArea);

    renderArea->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
/* Added the following on_actionAdd_Camera_triggered() function to provide menu item for user to insert camera specifications. */
void MainWindow::on_actionAdd_Camera_triggered()
{
    //QApplication::quit();
    // Create an instance of camera window
    CameraWindow *cameraWindow = new CameraWindow();
    cameraWindow->show();
    //cameraWindow->raise();
    //cameraWindow->setFocus();

}
