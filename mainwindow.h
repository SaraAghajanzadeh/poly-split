#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "renderarea.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAdd_Camera_triggered();

private:
    Ui::MainWindow *ui;

    RenderArea *renderArea;

};

#endif // MAINWINDOW_H
