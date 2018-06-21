#include "gldrawutil.h"
#include <QtMath>
#include <QOpenGLFunctions>
#include <QDebug>
void GLDrawUtil::drawArrow2D(const QVector2D &sv, const QVector2D &ev)
{
    const float arrowhead = 15.f;
    const double angle_arrow_head = M_PI/4.f;
    QVector2D vecO = sv;
    QVector2D vecA = ev;
    QVector2D vecOA =vecA-vecO;
    QVector2D vecP(vecOA.length(), 0);
    QVector2D vecQ(vecP.x()-arrowhead*cos(angle_arrow_head), arrowhead*sin(angle_arrow_head));
    QVector2D vecR(vecP.x()-arrowhead*cos(angle_arrow_head), -arrowhead*sin(angle_arrow_head));
    double rot_angle = atan2(vecOA.y(), vecOA.x())*180/M_PI;

    glPushMatrix();
    glTranslated(vecO.x(), vecO.y(), 0);
    glRotated(rot_angle, 0,0,1);

//    glBegin(GL_LINE_STRIP);

//    glVertex2d(0, 0);
//    glVertex2d(vecP.x(), vecP.y());

//    glVertex2d(vecP.x(), vecP.y());
//    glVertex2d(vecQ.x(), vecQ.y());

//    glVertex2d(vecP.x(), vecP.y());
//    glVertex2d(vecR.x(), vecR.y());

    glEnd();
    glBegin(GL_LINE_STRIP);

    glVertex2d(0, 0);
    glVertex2d(vecP.x(), vecP.y());

    glEnd();

    glBegin(GL_TRIANGLES);

    glVertex2d(vecP.x(), vecP.y());
    glVertex2d(vecQ.x(), vecQ.y());
    glVertex2d(vecR.x(), vecR.y());

    glEnd();

    glPopMatrix();
}
void GLDrawUtil::drawArrowHead2D(const QVector2D &sv, const QVector2D &ev)
{
    const float arrowhead = 15.f;
    const double angle_arrow_head = M_PI/4.f;
    QVector2D vecO = sv;
    QVector2D vecA = ev;
    QVector2D vecOA =vecA-vecO;
    QVector2D vecP(vecOA.length(), 0);
    QVector2D vecQ(vecP.x()-arrowhead*cos(angle_arrow_head), arrowhead*sin(angle_arrow_head));
    QVector2D vecR(vecP.x()-arrowhead*cos(angle_arrow_head), -arrowhead*sin(angle_arrow_head));
    double rot_angle = atan2(vecOA.y(), vecOA.x())*180/M_PI;

    glPushMatrix();
    glTranslated(vecO.x(), vecO.y(), 0);
    glRotated(rot_angle, 0,0,1);


    glBegin(GL_TRIANGLES);

    glVertex2d(vecP.x(), vecP.y());
    glVertex2d(vecQ.x(), vecQ.y());
    glVertex2d(vecR.x(), vecR.y());

    glEnd();

    glPopMatrix();
}


void GLDrawUtil::drawLine2D(const QVector2D &sv, const QVector2D &ev)
{
    glBegin(GL_LINE_STRIP);
    glVertex2d(sv.x(), sv.y());
    glVertex2d(ev.x(), ev.y());
    glEnd();
}

void GLDrawUtil::drawCircle2D(const QVector2D &cv, const double r)
{
    static const int N = 64;

    glBegin(GL_LINE_STRIP);
    for(int i = 0; i <= N; ++i){
        double theta = 2*M_PI*i / (double)N;
        QVector2D v  = cv + QVector2D(r*cos(theta), r*sin(theta));
        glVertex2d(v.x(), v.y());
    }
    glEnd();
}

void GLDrawUtil::drawFilledCircle2D(const QVector2D &cv, const double r)
{
    static const int N = 64;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(cv.x(), cv.y());
    for(int i = 0; i <= N; ++i){
        double theta = 2*M_PI*i / (double)N;
        QVector2D v  = cv + QVector2D(r*cos(theta), r*sin(theta));
        glVertex2d(v.x(), v.y());
    }
    glEnd();
}

void GLDrawUtil::drawHalfCircle2D(const QVector2D &cv, const double r)
{
    static const int N = 64;

    glBegin(GL_LINE_STRIP);
    for(int i = 0; i <= N; ++i){
        double theta = M_PI*i / (double)N;
        QVector2D v  = cv + QVector2D(r*cos(theta), r*sin(theta));
        glVertex2d(v.x(), v.y());
    }
    glEnd();
}

void GLDrawUtil::drawCircleArrow2D(const QVector2D &cv, const double r)
{
    glPushMatrix();
    glTranslated(cv.x(), cv.y(), 0);
    drawCircle2D(QVector2D(0,0), r);
    glBegin(GL_LINE_STRIP);
    glVertex2d(0, r);
    glVertex2d(r*cos(M_PI/4), r+r*sin(M_PI/4));

    glVertex2d(0, r);
    glVertex2d(r*cos(M_PI/4), r-r*sin(M_PI/4));
    glEnd();
    glPopMatrix();
}

void GLDrawUtil::drawSinCurve2D(const QVector2D &sv, const QVector2D &ev)
{
    glPushMatrix();
    static const int N = 64;
    double angle = atan2((ev-sv).y(),(ev-sv).x())*360/M_PI/2;
    glTranslated(sv.x(), sv.y(), 0);
    glRotated(angle, 0,0,1);
    double dx  = (ev-sv).length()/N;
    double h = 3*(ev-sv).length()/N;
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i <N; ++i){
        double theta  = 2 * M_PI * i / N;
        QVector2D v(dx*i, h * sin(theta));
        glVertex2d(v.x(), v.y());
    }
    glEnd();
    glPopMatrix();
}

void GLDrawUtil::drawSinArrow2D(const QVector2D &sv, const QVector2D &ev)
{
    glPushMatrix();
    static const int N = 64;
    double angle = atan2((ev-sv).y(),(ev-sv).x())*360/M_PI/2;
    glTranslated(sv.x(), sv.y(), 0);
    glRotated(angle, 0,0,1);
    double dx  = (ev-sv).length()/N;
    double h = 3*(ev-sv).length()/N;
    glBegin(GL_LINE_STRIP);
    QVector2D lastV;
    for(int i = 0; i <N; ++i){
        double theta  = 2 * M_PI * i / N;
        QVector2D v(dx*i, h * sin(theta));
        glVertex2d(v.x(), v.y());
        lastV = v;
    }
    glEnd();
    drawArrowHead2D(lastV, QVector2D(dx*N, 0));
    glPopMatrix();
}

void GLDrawUtil::drawBezierCurve2D(const QVector2D &v1, const QVector2D &v2, const QVector2D &v3, const QVector2D &v4)
{
    static const int N = 512;
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i<N; ++i){
        double t  = (double)i/N;
        double x = (1-t)*(1-t)*(1-t)*v1.x()+3*(1-t)*(1-t)*t*v2.x()+3*(1-t)*t*t*v3.x()+t*t*t*v4.x();
        double y = (1-t)*(1-t)*(1-t)*v1.y()+3*(1-t)*(1-t)*t*v2.y()+3*(1-t)*t*t*v3.y()+t*t*t*v4.y();
        glVertex2d(x,y);
    }
    glEnd();
}

void GLDrawUtil::drawBezierArrow2D(const QVector2D &v1, const QVector2D &v2, const QVector2D &v3, const QVector2D &v4)
{
    static const int N = 512;
    QVector2D lastV;
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i<N; ++i){
        double t  = (double)i/N;
        double x = (1-t)*(1-t)*(1-t)*v1.x()+3*(1-t)*(1-t)*t*v2.x()+3*(1-t)*t*t*v3.x()+t*t*t*v4.x();
        double y = (1-t)*(1-t)*(1-t)*v1.y()+3*(1-t)*(1-t)*t*v2.y()+3*(1-t)*t*t*v3.y()+t*t*t*v4.y();
        glVertex2d(x,y);
        lastV = QVector2D(x,y);
    }
    glEnd();
    drawArrowHead2D(lastV, v4);
}

void GLDrawUtil::drawRect3D(const int w, const int h)
{
    glBegin(GL_LINE_LOOP);
    glVertex3d(0,0,0);
    glVertex3d(0,h,0);
    glVertex3d(w,h,0);
    glVertex3d(w,0,0);
    glEnd();
}
