#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <QMainWindow>
#include "renderarea.h"

/* Added the following CameraWindow class to request camera specifications from the user.*/
namespace Ui {
class CameraWindow;
}

class CameraWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CameraWindow(QWidget *parent = nullptr);
    ~CameraWindow();

private slots:
    void on_done_button_clicked();

    void on_camera_horizontal_resolution_text_textChanged();

private:
    Ui::CameraWindow *ui;
    RenderArea renderarea;
};

#endif // CAMERAWINDOW_H
