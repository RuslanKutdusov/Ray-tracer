#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPaintEvent>
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

//    Matrix t = Matrix::TranslateMatrix(1, 2, 3);
//    t = t*Matrix::RotateX(2);
//    Matrix tt = t.inverse();
//    Matrix e = t*tt;
//    Vector v = Vector(2, -2, 0);
//    v = t.mul(v);
//    Vector v1 = tt.mul(v);
    ui->setupUi(this);
    drawed = false;
    //XZ1
    double box_size = 6;
    Material m3(Color(), Color(0.7, 0.2, 0.2), Color(0.7, 0.2, 0.2), 5, 8, 0, 0);
    Material m4(Color(), Color(0.2, 0.7, 0.2), Color(0.2, 0.7, 0.2), 5, 8, 0, 0);
    Material m1(Color(), Color(0.3, 0.6, 0.5), Color(0.3, 0.6, 0.5), 5, 8, 0, 0);
//    objects.push_back(new ObjectPlane(Vector(-box_size, -box_size,  box_size),
//                                      Vector( box_size, -box_size,  box_size),
//                                      Vector(-box_size, -box_size, -box_size),
//                                      m3));
//    //XZ2
//    objects.push_back(new ObjectPlane(Vector( box_size, box_size,  box_size),
//                                      Vector(-box_size, box_size,  box_size),
//                                      Vector( box_size, box_size, -box_size),
//                                      m4));
//    //YZ_far
//    objects.push_back(new ObjectPlane(Vector(-box_size, box_size, box_size),
//                                      Vector(-box_size, -box_size, box_size),
//                                      Vector(-box_size, box_size, -box_size),
//                                      m1));
//    //YZ_near
//    objects.push_back(new ObjectPlane(Vector(box_size, -box_size, box_size),
//                                      Vector(box_size, box_size, box_size),
//                                      Vector(box_size, -box_size, -box_size),
//                                      m3));
//    //XY_bottom
//    objects.push_back(new ObjectPlane(Vector(box_size, box_size,  -5),
//                                      Vector(-box_size, box_size, -5),
//                                      Vector(box_size, -box_size, -5),
//                                      m1));
//    //XY_top
//    objects.push_back(new ObjectPlane(Vector(-box_size,  box_size, box_size + EPSILON),
//                                      Vector( box_size,  box_size, box_size + EPSILON),
//                                      Vector(-box_size, -box_size, box_size + EPSILON),
//                                      m1));

    //objects.push_back(new ObjectPlane(Vector(0, 0, 3), 12, 12, 0, PI / 2, 0 , m3, true));


    Material m5(Color(), Color(0.5, 0.5, 0.7), Color(0.5, 0.5, 0.7), 100, 2, 0, 1);
    Material m6(Color(), Color(0.5, 0.5, 0.5), Color(0.5, 0.5, 0.5), 0.1, 2, 1, 0.1);
    //objects.push_back(new ObjectPlane(Vector(0, 0, 0.1), 6, 6, 1.1, 0, 0 , m1));
//    double z = 0.3;
//    double y = 0.1;
//    double x = 0;
//    objects.push_back(new ObjectPlane(Vector(0, 0, 1.5),  3, 3, x,           y,            z , m1));
//    objects.push_back(new ObjectPlane(Vector(0, 0, -1.5), 3, 3, x,           y,            z , m1, true));
//    objects.push_back(new ObjectPlane(Vector(0, 0, 1.5),  3, 3, x,            PI / 2 + y,  z , m1));
//    objects.push_back(new ObjectPlane(Vector(0, 0, 1.5),  3, 3, x,           -PI / 2 + y,  z , m1));
//    objects.push_back(new ObjectPlane(Vector(0, 0, 1.5),  3, 3, PI / 2 + x,  y,            z , m1));
//    objects.push_back(new ObjectPlane(Vector(0, 0, 1.5),  3, 3, -PI / 2 + x, y,            z , m1));

    Matrix m = Matrix::TranslateMatrix(-4, 0, 0);
    m = Matrix::RotateY(PI / 2) * m;
    objects.push_back(new ObjectPlane(m, 15, 15, m3));

//    double x = 0.8;
//    double y = 0.2;
//    double z = 0.4;
//    Vector pos(0, 0, -1);
//    m = Matrix::TranslateMatrix(0, 0, -1.5);
//    m = m * Matrix::RotateX(x) * Matrix::RotateY(y) * Matrix::RotateZ(z) * Matrix::TranslateMatrix(pos);
//    objects.push_back(new ObjectPlane(m, 3, 3, m1));

//    m = Matrix::TranslateMatrix(0, 0, 1.5);
//    m = m * Matrix::RotateX(x) * Matrix::RotateY(y) * Matrix::RotateZ(z) * Matrix::TranslateMatrix(pos);
//    objects.push_back(new ObjectPlane(m, 3, 3, m1));

//    m = Matrix::TranslateMatrix(-1.5, 0, 0);
//    m = Matrix::RotateY(-PI / 2) * m;
//    m = m * Matrix::RotateX(x) * Matrix::RotateY(y) * Matrix::RotateZ(z) * Matrix::TranslateMatrix(pos);
//    objects.push_back(new ObjectPlane(m, 3, 3, m1));

//    m = Matrix::TranslateMatrix(1.5, 0, 0);
//    m = Matrix::RotateY(PI / 2) * m;
//    m = m * Matrix::RotateX(x) * Matrix::RotateY(y) * Matrix::RotateZ(z) * Matrix::TranslateMatrix(pos);
//    objects.push_back(new ObjectPlane(m, 3, 3, m1));

//    m = Matrix::TranslateMatrix(0, 1.5, 0);
//    m = Matrix::RotateX(-PI / 2) * m;
//    m = m * Matrix::RotateX(x) * Matrix::RotateY(y) * Matrix::RotateZ(z) * Matrix::TranslateMatrix(pos);
//    objects.push_back(new ObjectPlane(m, 3, 3, m1));

//    m = Matrix::TranslateMatrix(0, -1.5, 0);
//    m = Matrix::RotateX(PI / 2) * m;
//    m = m * Matrix::RotateX(x) * Matrix::RotateY(y) * Matrix::RotateZ(z) * Matrix::TranslateMatrix(pos);
//    objects.push_back(new ObjectPlane(m, 3, 3, m1));

    objects.push_back(new ObjectBox(Vector(0, 0, -3), Vector(0, 0, 1), 3, m1));

//    objects.push_back(new ObjectSphere(Vector(0, 0, 0), 3, m6));
    //objects.push_back(new ObjectPlane(Vector(0, 0, -2), 2, 2, 0, 0, 1, m1));

//    objects.push_back(new ObjectPlane(Vector(-3, 0, 0),
//                                      Vector(-3, 3, 0),
//                                      Vector(-3, 0, -3),
//                                      m6));

    //lights.push_back(ObjectLight(Vector(5, 5, 5), Color(0.7, 0.7, 0.8)));
    lights.push_back(ObjectLight(Vector(4, 0, 3), Color(0.8, 0.6, 0.7)));

    //lights.push_back(ObjectLight(Vector(-4, 4, 4), light_material));
//    Material light_material2(QColor(100, 0,0 ), QColor(0, 0, 0), QColor(0, 0, 0));
//    lights.push_back(ObjectLight(Vector(3,0,2), light_material2));
}

MainWindow::~MainWindow()
{
    delete ui;
}

#define MAX_DEPTH  20

Color MainWindow::ray_tracing(const Ray & ray, const int &depth, int & rays_count, double * distance){
    if (depth == MAX_DEPTH)
        return Color(0, 0, 0);
    Color ret(0, 0, 0);
    Intersection intr;
    int i_object = -1;
    double distance2obj = INFINITY;
    for(size_t i = 0; i < objects.size(); i++){
        Intersection in;
        if (objects[i]->RayIntersect(ray, in)){
            double dist = in.point.distance(ray.start_point);
            if (dist < distance2obj){
                distance2obj = dist;
                intr = in;
                i_object = i;
            }
        }
    }
    if (i_object == -1)
        return ret;

    rays_count++;

    if (distance)
        *distance = distance2obj;

    int depth_ = depth + 1;

    Color diffuse;
    Color specular;
    for(size_t i = 0; i < lights.size(); i++){
        double distance2light = lights[i].distance(intr.point);
        Ray to_light(lights[i].m_center, intr.point);

        //проверям, в тени какого либо объекта или нет
        bool i_object_in_shadow = false;
        for(size_t j = 0; j < objects.size(); j++){
            Intersection intr2;
            if (objects[j]->RayIntersect(to_light, intr2)){
                double distance_ = intr2.point.distance(intr.point);
                if (distance_ < distance2light){
                    i_object_in_shadow = true;
                    break;
                }
            }
        }
        if (i_object_in_shadow)
            continue;

        double angle_cos = to_light.vector.dot(intr.normal);
        if (angle_cos > 0){
            if (!objects[i_object]->m_material.m_diffuse.is_black())
                diffuse = diffuse + lights[i].m_color * (angle_cos );
            if (!objects[i_object]->m_material.m_specular.is_black())
                specular = specular + lights[i].m_color * pow(angle_cos, objects[i_object]->m_material.m_phong) ;
        }
    }

    double d = 0;

    Color reflect_ray_color;
    //if (objects[i_object]->m_material.m_refract_amount < 0.8)
    {
//        reflect_ray_color = ray_tracing(intr.reflect_ray, depth_, rays_count, &d);
//        reflect_ray_color = reflect_ray_color * exp(-objects[i_object]->m_material.m_beta);
    }

    Color refract_ray_color;
    if (objects[i_object]->m_material.m_refract_amount > 0)
        refract_ray_color = ray_tracing(intr.refract_ray, depth_, rays_count, NULL);

    ret = objects[i_object]->m_material.m_ambient +
          objects[i_object]->m_material.m_diffuse * diffuse +
          objects[i_object]->m_material.m_specular * specular +
            reflect_ray_color +
            refract_ray_color;
    //fflush(stdout);
    return ret;
}

void MainWindow::paintEvent(QPaintEvent *pevent){
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(pevent->rect(), QColor(0, 0, 0));
//    painter.translate(pevent->rect().width() / 2, pevent->rect().height() / 2);
//    painter.drawPoint(0, 0);
    double ratio = (double)pevent->rect().width() / (double)pevent->rect().height();
    Vector camera_pos(5.8, 0, 0);
    double w = 6;
    double h = w / ratio;
    Viewport viewport(Vector(4, -w/2, h/2),
                      Vector(4, w/2, h/2),
                      Vector(4, -w/2, -h/2),
                      Vector(4, w/2, -h/2));
    double step_y = w / pevent->rect().width();

    for(double y = viewport.m_p1.y, xx = 0; y < viewport.m_p2.y; y += step_y, xx++){
        int rays_count = 0;
        for(double z = viewport.m_p1.z, yy = 0; z > viewport.m_p3.z; z -= step_y, yy++){
            Vector viewport_point(viewport.m_p1.x, y, z);
            Ray first_ray(viewport_point, camera_pos);
            Color c =ray_tracing(first_ray, 0, rays_count, NULL);
            QPen p = QPen(c.convert());
            painter.setPen(p);
            painter.drawPoint(xx, yy);
        }
        printf("%d/%d rays=%d\n", (int)xx, pevent->rect().width(), rays_count);
        fflush(stdout);
    }
    painter.end();
}
