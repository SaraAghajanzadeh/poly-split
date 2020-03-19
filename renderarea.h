#ifndef RENDERAREA_H
#define RENDERAREA_H
#include"vector.h"

#include <QWidget>
#include <map>

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);
    /* Added the following functions setFieldofCoverage(double) and getFieldofCoverage() and static data member fieldofCoverage to get camera's field of coverage area from user input via camera window class.*/
    std::map<std::string,double> getCamera();
    void setCamera(double, double, double, double, double);
    static std::map<std::string,double> camera;

    int static calculateCoverage(std::vector<QPolygon> fovs);
    int static calculateOverlap(std::vector<QPolygon> fovs);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    void initPolygons(void);
};

#endif // RENDERAREA_H
