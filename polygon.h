#ifndef POLYGON_H
#define POLYGON_H

#include <line.h>
#include <vector.h>
#include <QPen>
#include<iostream>
#include <math.h>

class Polygon
{
private:
    Vectors poly;

public:
    Polygon();

    Polygon(const Vectors &v);

    /* Added the following fieldofCoverage() function to compute camera field of coverage given camera specifications. */
    static QPolygon fieldofCoverage(qreal angle, qreal length, Vector start, int hfov)
    {
        QPolygon triangle; // triangle polygon
        QVector<QPoint> points; // points of the triangle polygon

        QLineF angleline1; // triangle side 1
        QLineF angleline2; // triangle side 2
        QLineF anglebisector; // "distance" to object (triangle height)
        QLineF base; // camera horizontal field of coverage (triangle base)
        int side = std::sqrt(std::pow(length,2)+std::pow(hfov/2,2));
        anglebisector.setP1(QPointF(start.x,start.y));
        anglebisector.setLength(length);
        anglebisector.setAngle(0);

        angleline2.setP1(QPointF(start.x,start.y));
        angleline2.setLength(side);
        angleline2.setAngle(angle/2);

        angleline1.setP1(QPointF(start.x,start.y));
        angleline1.setLength(side);
        angleline1.setAngle(-angle/2);

        QPointF endpoint1 = angleline1.p2();
        QPointF endpoint2 = angleline2.p2();
        base.setLine(endpoint1.x(), endpoint1.y(),endpoint2.x(),endpoint2.y());

        QPointF startpoint = anglebisector.p1();
        QPointF startpoint1 = base.p1();
        QPointF startpoint2 = base.p2();

        points.push_back(startpoint.toPoint());
        points.push_back(endpoint1.toPoint());
        points.push_back(endpoint2.toPoint());

        /*std::cout<<startpoint.toPoint().x()<<std::endl;
        std::cout<<startpoint.toPoint().y()<<std::endl;
        std::cout<<endpoint1.toPoint().x() <<std::endl;
        std::cout<<endpoint1.toPoint().y()<<std::endl;
        std::cout<<endpoint2.toPoint().x() <<std::endl;
        std::cout<<endpoint2.toPoint().y()<<std::endl;*/
        triangle = QPolygon(points);
        return triangle;

    }

    /* rotatePolygon() is my modified version of rotatePolygon() at https://github.com/sccheah/ECS175-P1/blob/master/polygons.cpp*/
    static QPolygon rotatePolygon(Vector start, QPolygon selected_polygon, double theta)
    {
        QPolygon new_triangle; // triangle polygon
        QVector<QPoint> new_points; // points of the triangle polygon
        double pivot_x = start.x;
        double pivot_y = start.y;

        double x;
        double y;

        for (int i = 0; i < selected_polygon.size(); i++)
        {
            QPoint pt = selected_polygon.point(i);

            /*std::cout<<"Original x coordinate of the polygon:"<<std::endl;
            std::cout<<pt.x()<<std::endl;
            std::cout<<"Original y coordinate of the polygon:"<<std::endl;
            std::cout<<pt.y()<<std::endl;*/

            x = pivot_x + ((((double)(pt.x()) - pivot_x) * cos(theta)) - (((double)(pt.y()) - pivot_y) * sin(theta)));
            y = pivot_y + ((((double)(pt.x()) - pivot_x) * sin(theta)) + (((double)(pt.y()) - pivot_y) * cos(theta)));

            pt.setX(x);
            pt.setY(y);

            /*std::cout<<"New x coordinate of the polygon:"<<std::endl;
            std::cout<<pt.x()<<std::endl;
            std::cout<<"New y coordinate of the polygon:"<<std::endl;
            std::cout<<pt.y()<<std::endl;
            new_points.push_back(pt);*/
            new_points.push_back(pt);
        }
        new_triangle = QPolygon(new_points);
        return new_triangle;
    }


    double countSquare(void) const;
    double countSquare_signed(void) const;

    int split(double square, Polygon &poly1, Polygon &poly2, Line &cutLine) const;

    double findDistance(const Vector &point) const;
    Vector findNearestPoint(const Vector &point) const;

    Vector countCenter(void) const;

    void splitNearestEdge(const Vector &point);

    int isPointInside(const Vector &point) const;
    int isClockwise(void) const;

    const Vectors &getVectors(void) const
    {
        return poly;
    }

    void push_back(const Vector &v)
    {
        poly.push_back(v);
    }

    int empty(void) const
    {
        return poly.empty();
    }

    Vector &operator [](size_t index)
    {
        return poly[index];
    }

    Polygon &operator =(const Polygon &p)
    {
        poly = p.poly;
        return *this;
    }

    Vector operator [](size_t index) const
    {
        return poly[index];
    }

    void clear(void)
    {
        poly.clear();
    }

    size_t size(void) const
    {
        return poly.size();
    }
};

#endif // POLYGON_H
