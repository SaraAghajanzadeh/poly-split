#include "camerawindow.h"
#include "ui_camerawindow.h"
/* Added the following CameraWindow class to request camera specifications from the user.*/

CameraWindow::CameraWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraWindow)
{
    ui->setupUi(this);

}

CameraWindow::~CameraWindow()
{
    delete ui;
}

void CameraWindow::on_done_button_clicked()
{
    double camera_horizontal_resolution = ui->camera_horizontal_resolution_text->toPlainText().toDouble();
    //int camera_vertical_resolution = ui->camera_vertical_resolution_text->toPlainText().toDouble();
    double camera_focal_length = ui->camera_focal_length_text->toPlainText().toDouble();
    double camera_angle_view = ui->camera_angle_view_text->toPlainText().toDouble();
    double camera_chip_width = ui->camera_chip_width_text->toPlainText().toDouble();
    double ppf_threshold = ui->ppf_threshold_text->toPlainText().toDouble();
    double distance = (camera_focal_length * camera_horizontal_resolution)/ (camera_chip_width * ppf_threshold);
    QString dist = QString::number(distance);
    ui->distance_text->setTextColor("green");
    ui->distance_text->setText(dist);
    double field_of_view = camera_horizontal_resolution / ppf_threshold;
    double coverage = (distance * field_of_view) / 2;
    QString field_of_coverage = QString::number(coverage);
    ui->foc_text->setTextColor("green");
    ui->foc_text->setText(field_of_coverage);
    renderarea.setCamera(coverage, distance, camera_angle_view, camera_horizontal_resolution, ppf_threshold);
}

void CameraWindow::on_camera_horizontal_resolution_text_textChanged()
{
    ui->camera_horizontal_resolution_text->setTextColor("black");
    ui->camera_vertical_resolution_text->setTextColor("black");
    ui->camera_focal_length_text->setTextColor("black");
    ui->camera_angle_view_text->setTextColor("black");
    ui->camera_chip_width_text->setTextColor("black");
    ui->ppf_threshold_text->setTextColor("black");
}
