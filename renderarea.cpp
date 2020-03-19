#include "renderarea.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

#include <cstdio>
#include <cfloat>

#include <line.h>
#include <vector.h>
#include <polygon.h>
/*
 Added the following header files:
 fstream
 iostream

 Added the following data structures:
 pointsMainPoly - all points in the main polygon
 fieldofCoverage static data member in renderarea class should be re declared also here.
 cutPolygon - all the polygons cut from the main polygon are stored in cutPolygons data structure.
 focs - triangular field of coverage shapes for all vertices of all subpolygons
 new_focs - rotated focs w.r.t angle theta
 final_focs - best (greedy) foc for each subpolygon
 intersectedVectors - intersection between new_focs and their respective subpolygons
 localPlacementScore: area of intersectedVectors - maximum score is located in the beginning of the localPlacementScore data structure.
 Note: subpolygon is the QPolygon version of the Polygon class
 */
#include<fstream>
#include<iostream>

QVector<QPoint> pointsMainPoly;
std::map<std::string,double> RenderArea::camera;
std::vector<Polygon> cutPolygons;
std::vector<QPolygon>subpolygons;
std::vector<QPolygon>focs;
std::vector<QPolygon>new_focs;
std::vector<QPolygon>overlaps;
std::vector<Vector> intersectedVectors;
std::map<int, QPolygon, std::greater <int>> localPlacementScore;

// Need to be global for drawing purposes:
std::vector<Polygon> polygons;
std::vector<QColor> polygons_colors;
std::vector<QPolygon>final_focs;

double squareToCut;
int selectedPolygon;
int showInfo = 0;
int showHelp = 1;

double scale = 1;
double offset_x = 0;
double offset_y = 0;

int mouseLeftPress = 0;
int mouse_x = 0;
int mouse_y = 0;
Vector mouse;

double pointSize = 5;

int selectedPoint = -1;

const QChar degreeChar(0260);

void drawPoly(QPainter &painter, const Polygon &poly)
{
    QPolygonF polypon;
    for(unsigned i = 0; i < poly.size(); i++)
    {
        polypon << QPointF(poly[i].x, poly[i].y);
    }
    painter.drawPolygon(polypon);
}
/* Added the following drawText function */
void drawText(QPainter &painter)
{
    if(showHelp)
    {
        int y = 0, dy = painter.fontMetrics().height() + 1.0;
        painter.drawText(10, y += dy, "Cut area square: " + QString::number(squareToCut));
        painter.drawText(10, y += dy, "Q/W - increase/decrease cut area square to 100");
        painter.drawText(10, y += dy, "q/w - increase/decrease cut area square to 10");
        painter.drawText(10, y += dy, "a/s - swith between areas");
        painter.drawText(10, y += dy, "r - to restore initial polygon");
        painter.drawText(10, y += dy, "i - show/hide polygons square value");
        painter.drawText(10, y += dy, "c - to cut area as black cut line shows");
        painter.drawText(10, y += dy, "h - show/hide this text");
        painter.drawText(10, y += dy, "Mouse wheel to adjust scale");
        painter.drawText(10, y += dy, "Left mouse click and drag'n'drop on background to move all scene");
        painter.drawText(10, y += dy, "Left mouse click and drag'n'drop on vertex to move vertices");
        painter.drawText(10, y += dy, "Right mouse click to select nearest polygon");
        painter.drawText(10, y += dy, "Middle mouse click to split edge of selected polygon");
    }
}

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);

    polygons_colors.push_back(Qt::darkRed);
    polygons_colors.push_back(Qt::green);
    polygons_colors.push_back(Qt::darkGreen);
    polygons_colors.push_back(Qt::blue);
    polygons_colors.push_back(Qt::darkBlue);
    polygons_colors.push_back(Qt::cyan);
    polygons_colors.push_back(Qt::darkCyan);
    polygons_colors.push_back(Qt::magenta);
    polygons_colors.push_back(Qt::darkMagenta);
    polygons_colors.push_back(Qt::darkYellow);
    polygons_colors.push_back(Qt::gray);
    polygons_colors.push_back(Qt::darkGray);

    initPolygons();

}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    //drawText(painter); Originally drawing text was done here, but it would not draw text. It is now called further below.

    painter.save();
    painter.translate(offset_x, offset_y);
    painter.scale(scale, scale);

    painter.setRenderHint(QPainter::Antialiasing, true);

    for(size_t i = 0; i < polygons.size(); i++)
    {
        painter.setPen(QPen(polygons_colors[i % polygons_colors.size()], 1));
        QColor c = QColor(polygons_colors[i % polygons_colors.size()]);
        c.setAlpha(40);
        painter.setBrush(c);
        drawPoly(painter, polygons[i]);

        if(showInfo)
        {
            Vector p = polygons[i].countCenter();
            painter.drawText(QPointF(p.x, p.y), QString::number(polygons[i].countSquare()));
        }
    }
    drawText(painter);

    Polygon poly1, poly2;
    Line cut;
    if(polygons[selectedPolygon].split(squareToCut, poly1, poly2, cut))
    {
        painter.setPen(QPen(Qt::black, 1.5));
        painter.drawLine(QPointF(cut.getStart().x, cut.getStart().y), QPointF(cut.getEnd().x, cut.getEnd().y));
    }

    if(showInfo)
    {
        painter.setPen(QPen(QColor(0, 0, 0, 50), 1));
        Vector np = polygons[selectedPolygon].findNearestPoint(mouse);
        painter.drawLine(QPointF(mouse.x, mouse.y), QPointF(np.x, np.y));
    }

    painter.setPen(QPen(QColor(250, 0, 0, 100), 3));
    painter.setBrush(Qt::transparent);
    drawPoly(painter, polygons[selectedPolygon]);
    /*Added the following:
     * Vector::angle() function to determine the angle for all polygon vertices.
    */
    for(size_t i = 0; i < polygons[selectedPolygon].size(); i++)
    {
        //painter.drawEllipse(QPointF(p.x, p.y), pointSize, pointSize);
        size_t pos2 = i+1;
        size_t pos3 = i+2;
        if(pos2 >= polygons[selectedPolygon].size())
        {
                pos2 = pos2-polygons[selectedPolygon].size();
        }
        if(pos3 >= polygons[selectedPolygon].size())
        {
                pos3 = pos3-polygons[selectedPolygon].size();
        }
        Vector p = polygons[selectedPolygon][i];
        Vector p2 = polygons[selectedPolygon][pos2];
        Vector p3 = polygons[selectedPolygon][pos3];


        int angle = Vector::angle(p,p2,p3);
        //Vector p = polygons[selectedPolygon][i];
        //std::cout<< i<<" "<<Vector::angle(p,p2,p3)<<p<<p2<<p3<<std::endl;
        //painter.drawText(QPointF(p2.x,p2.y),QString::number(qRound(Vector::angle(p,p2,p3))).append(degreeChar));
    }

    QBrush brush;
    QPainterPath path;
    brush.setColor(Qt::darkGray);
    brush.setStyle(Qt::SolidPattern);
    //Displaying initial fovs
    /*if(!focs.empty())
    {
        int size = focs.size();
        std::cout<<"initial focs size: "<<std::endl;
        std::cout<<size<<std::endl;
        for(int i=0; i<size; i++)
        {
            //new_focs.push_back(Polygon::rotatePolygon(focs[i],90)); //rotate 90 degrees
            path.addPolygon(focs[i]);
            painter.fillPath(path, brush);
        }
    }
    //Displaying new (rotated w.r.t angle) fovs
    if(!new_focs.empty())
    {
        std::cout<<"came in painter's new_focs"<<std::endl;

        int size = new_focs.size();
        std::cout<<"rotated focs size: "<<std::endl;
        std::cout<<size<<std::endl;
        for(int i=0; i<size; i++)
        {
            path.addPolygon(new_focs[i]);
            painter.fillPath(path, brush);
        }
    }*/
    // Displaying final focs
    if(!final_focs.empty())
    {
        int size = final_focs.size();
        //std::cout<<"final focs size: "<<std::endl;
        std::cout<<size<<std::endl;
        for(int i=0; i<size; i++)
        {
            path.addPolygon(final_focs[i]);
            painter.fillPath(path, brush);
        }
    }

    painter.restore();

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void RenderArea::keyPressEvent(QKeyEvent *event)
{
    //squareToCut = 12800;
    squareToCut = getCamera().find("foc")->second;
    double d = getCamera().find("distance")->second;
    double a = getCamera().find("angle")->second;
    double hfov = getCamera().find("hfov")->second;
    // Key Q changes starting vertex.
    if(event->key() == Qt::Key_Q)
    {
        squareToCut += event->modifiers() == Qt::SHIFT ? 100 : 10;
        repaint();
    }
    // Key W determines the line of cut for the subpolygon already cut.
    if(event->key() == Qt::Key_W)
    {
        double t = squareToCut - (event->modifiers() == Qt::SHIFT ? 100 : 10);
        squareToCut = t < 10 ? 10 : t;
        repaint();
    }

    // Modified Key C to cut and keep track of cuted polygons (subpolygons).
    if(event->key() == Qt::Key_C)
    {
        Polygon poly1, poly2;
        Line cut;
        if(polygons[selectedPolygon].split(squareToCut, poly1, poly2, cut))
        {

            if(poly1.countSquare() < poly2.countSquare())
            {
                polygons[selectedPolygon] = poly1;
                polygons.push_back(poly2);
                cutPolygons.push_back(poly1);
                selectedPolygon = polygons.size() - 1;
            }

            if (!(poly1.countSquare() < poly2.countSquare()))
            {
                polygons[selectedPolygon] = poly2;
                polygons.push_back(poly1);
                cutPolygons.push_back(poly2);
                selectedPolygon = polygons.size() - 1;
            }

            repaint();
        }
    }
    // Added Key K to print coordinates of the cuted polygons.
    if(event->key() == Qt::Key_K)
    {
        //std::cout<<cutPolygons.size()<<std::endl;
        std::cout<<cutPolygons[--selectedPolygon].size()<<std::endl; //Initially selectedPolygon is 1, then 0. Equal to j.
        QPolygon subpolygon; // equivalent to cutPolygons[j]
        QVector<QPoint> subpoints;
        for(size_t j = 0; j < cutPolygons.size(); j++)
        {
            for(size_t i = 0; i < cutPolygons[j].size(); i++)
            {
                Vector p = cutPolygons[j][i];
                //fprintf(stdout, "polygons[0].push_back(Vector(%.2e, %.2e));\n", 50, p.x, 50, p.y);
                subpoints.push_back(QPoint(p.x,p.y));
                /* fieldofCoverage function in Polygon class returns a camera foc subject to camera specifications. */
                QPolygon mytriangle = Polygon::fieldofCoverage(a,d,p, hfov);
                int first_degree = 30;
                for (int d = 1; d<12; d++)
                {
                    QPolygon new_foc = Polygon::rotatePolygon(p,mytriangle,first_degree*d); //rotate every (first_degree) degrees
                    new_focs.push_back(new_foc);

                }
                //focs.push_back(mytriangle);

            }
            std::cout<<"\n"<<std::endl;

            subpolygon = QPolygon(subpoints);
            subpolygons.push_back(subpolygon);
            subpoints.clear();
            /* Now, given the rotated focs, we want to find the foc with best intersection with its corresponding subpolygon.*/
            QPolygon intersection;
            double areaScore = 0;
            if(!new_focs.empty())
            {
                int size = new_focs.size();
                for(int new_foc=0; new_foc<size; new_foc++)
                {
                    intersection = subpolygon.intersected(new_focs[new_foc]);
                    for(int n=0; n<intersection.size();n++)
                    {
                        int x = intersection[n].x();
                        int y = intersection[n].y();
                        Vector v = Vector(x,y);
                        intersectedVectors.push_back(v);
                    }
                    areaScore = Polygon(intersectedVectors).countSquare();
                    intersectedVectors.clear();
                    localPlacementScore.insert(std::make_pair(areaScore,new_focs[new_foc]));
                }
            }
            final_focs.push_back(localPlacementScore.begin()->second);
            localPlacementScore.clear();

        }

        selectedPolygon+=1;
        fflush(stdout);
        repaint();
    }

    // Key P prints coordinates of the selected polygon.
    if(event->key() == Qt::Key_P)
    {
        for(size_t i = 0; i < polygons[selectedPolygon].size(); i++)
        {
            Vector p = polygons[selectedPolygon][i];
            fprintf(stdout, "polygons[0].push_back(Vector(%.*e, %.*e));\n", 50, p.x, 50, p.y);
        }
        fflush(stdout);
    }
    // Key A selects another subpolygon that is just cut.
    if(event->key() == Qt::Key_A)
    {
        if(selectedPolygon > 0)
        {
            selectedPolygon--;
        }
        repaint();
    }
    // Key S selects the current polygon left for further cutting.
    if(event->key() == Qt::Key_S)
    {
        if(selectedPolygon < (int)polygons.size() - 1)
        {
            selectedPolygon++;
        }
        repaint();
    }
    // Key R resets everything.
    if(event->key() == Qt::Key_R)
    {
        initPolygons();
        repaint();
    }
    // Key I displays areas' values.
    if(event->key() == Qt::Key_I)
    {
        showInfo = !showInfo;
        repaint();
    }
    // Key H
    if(event->key() == Qt::Key_H)
    {
        showHelp = !showHelp;
        repaint();
    }

    if(event->key() == Qt::Key_F)
    {
        int coverage_score, overlap_score = 0;
        coverage_score = RenderArea::calculateCoverage(final_focs);
        std::cout<<"coverage score:"<<coverage_score<<std::endl;
        overlap_score = RenderArea::calculateOverlap(final_focs);
        std::cout<<"overlap score:"<<overlap_score<<std::endl;
    }

}


void RenderArea::mousePressEvent(QMouseEvent *event)
{
    /*mouse_x = event->x();
    mouse_y = event->y();

    mouse = Vector((event->x() - offset_x) / scale, (event->y() - offset_y) / scale);

    if(event->button() == Qt::LeftButton)
    {
        for(size_t i = 0; i < polygons[selectedPolygon].size(); i++)
        {
            Vector p = polygons[selectedPolygon][i];
            if((mouse - p).length() < pointSize)
            {
                selectedPoint = i;
            }
        }

        mouseLeftPress = 1;
    }
    if(event->button() == Qt::MiddleButton)
    {
        polygons[selectedPolygon].splitNearestEdge(mouse);
    }
    if(event->button() == Qt::RightButton)
    {
        double minDist = DBL_MAX;
        for(size_t i = 0; i < polygons.size(); i++)
        {
            double dist = polygons[i].findDistance(mouse);
            if(dist < minDist)
            {
                minDist = dist;
                selectedPolygon = i;
            }
        }
        squareToCut = polygons[selectedPolygon].countSquare() / 2.0;
        repaint();
    }*/
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
    /*if(mouseLeftPress && selectedPoint != -1)
    {
        polygons[selectedPolygon][selectedPoint].x = polygons[selectedPolygon][selectedPoint].x + (event->x() - mouse_x) / scale;
        polygons[selectedPolygon][selectedPoint].y = polygons[selectedPolygon][selectedPoint].y + (event->y() - mouse_y) / scale;
    }
    else if(mouseLeftPress)
    {
        offset_x = offset_x + (event->x() - mouse_x);
        offset_y = offset_y + (event->y() - mouse_y);
    }

    mouse_x = event->x();
    mouse_y = event->y();
    mouse = Vector((event->x() - offset_x) / scale, (event->y() - offset_y) / scale);
    repaint();*/
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    /*if(event->button() == Qt::LeftButton)
    {
        mouseLeftPress = 0;
        selectedPoint = -1;
    }*/
}

void RenderArea::wheelEvent(QWheelEvent *event)
{
    scale += 5 * scale / event->delta();
    repaint();
}


void RenderArea::initPolygons()
{
    polygons.clear();
    polygons.push_back(Polygon());

    polygons[0].push_back(Vector(450.0, 100.0));
    polygons[0].push_back(Vector(900.0, 100.0));
    polygons[0].push_back(Vector(900.0, 400.0));
    polygons[0].push_back(Vector(450.0, 400.0));
    QPoint tmppoint1;
    tmppoint1.setX(450.0);
    tmppoint1.setY(100.0);
    pointsMainPoly.push_back(tmppoint1);
    QPoint tmppoint2;
    tmppoint2.setX(900.0);
    tmppoint2.setY(100.0);
    pointsMainPoly.push_back(tmppoint2);
    QPoint tmppoint3;
    tmppoint3.setX(900.0);
    tmppoint3.setY(400.0);
    pointsMainPoly.push_back(tmppoint3);
    QPoint tmppoint4;
    tmppoint4.setX(450.0);
    tmppoint4.setY(400.0);
    pointsMainPoly.push_back(tmppoint4);

    /*


    // coordinates x and y
    int x, y;
    // QPoint object with coordinates x and y
    QPoint tmppoint;
    // Vector object with coordinates x and y
    Vector v;
    // create input stream object
    std::ifstream infile("/Users/saraaghajanzadeh/Desktop/coordinate.txt");
    if(!infile) {std::cout<<"can not open input file"<<std::endl;}
    // read contents of the input file (coordinates)
    while(infile >> x >> y){
        v = Vector(x,y);
        polygons[0].push_back(v);
        tmppoint.setX(x);
        tmppoint.setY(y);
        pointsMainPoly.push_back(tmppoint);
    }

    RenderArea::fieldofCoverage = 0;
    RenderArea::camera.insert(std::pair<std::string,double>("foc",0));
    squareToCut = 0;*/

    squareToCut = polygons[0].countSquare() / 47.0;
    selectedPolygon = 0;
}

std::map<std::string,double> RenderArea::getCamera()
{
    return RenderArea::camera;
}

void RenderArea::setCamera(double foc, double distance, double angle, double resolution, double threshold)
{

    RenderArea::camera.insert(std::pair<std::string,double>("foc",foc));
    RenderArea::camera.insert(std::pair<std::string,double>("distance",distance));
    RenderArea::camera.insert(std::pair<std::string,double>("angle",angle));
    // resolution refers to the horizontal resolution of cameras (ex. 1920 x 1080 -> 1920)
    RenderArea::camera.insert(std::pair<std::string,double>("resolution",resolution));
    // threshold is the minimum number of pixels that objects should occupy for successful detection and tracking.
    RenderArea::camera.insert(std::pair<std::string,double>("threshold",threshold));
    // camera horizontal field of view:
    double hfov = resolution / threshold;
    RenderArea::camera.insert(std::pair<std::string, double>("hfov",hfov));


}

int RenderArea::calculateOverlap(std::vector<QPolygon> fovs){
    QPolygon intersection;
    int overlapScore = 0;
    int size = fovs.size();
    for(int i=0; i<size;i++){
        for(int j=i+1;j<size;j++){
            if(fovs[i].intersects(fovs[j])){
                intersection = fovs[i].intersected(fovs[j]);
                overlaps.push_back(intersection);
            }
        }
    }
    overlapScore = RenderArea::calculateCoverage(overlaps);
    return overlapScore;
}
//need all coordinates of main polygon
int RenderArea::calculateCoverage(std::vector<QPolygon> fovs){
    QPolygon mainPolygon(pointsMainPoly);
    float totalPointsInsidePolygon =0;
    float coveredPoints = 0;
    float coverageScore = 0;
    QPoint star;
    for(int x=450;x<900;x++)
    {
        for(int y=100;y<400;y++)
        {

            if(mainPolygon.containsPoint(QPoint(x,y),Qt::WindingFill))
            {
                totalPointsInsidePolygon++;
                for(QPolygon fov: fovs)
                {
                    star.setX(x);
                    star.setY(y);
                    if(fov.containsPoint(star,Qt::WindingFill))
                    {
                        coveredPoints++;
                        break;
                    }
                }

            }
        }
    }
    std::cout<<"total points inside: "<<totalPointsInsidePolygon<<std::endl;
    std::cout<<"coveredPoints: "<<coveredPoints<<std::endl;
    coverageScore = coveredPoints/totalPointsInsidePolygon * 100;
    return coverageScore;
}

