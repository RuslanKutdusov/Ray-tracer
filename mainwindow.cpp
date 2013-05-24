#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPaintEvent>
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    buf = NULL;

    ui->setupUi(this);
    Material m1(Color(0.0, 0.0, 0.0), Color(0.003, 0.003, 0.003), Color(0.5, 0.5, 0.5), 5, 15, 0, 0, "wall.png");
    Material m2(Color(0.0, 0.1, 0.0), Color(0.1, 0.4, 0.1), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m3(Color(0.0, 0.0, 0.1), Color(0.1, 0.1, 0.4), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m4(Color(0.1, 0.1, 0.0), Color(0.4, 0.4, 0.1), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m5(Color(0.0, 0.1, 0.1), Color(0.1, 0.4, 0.4), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m6(Color(0.2, 0.2, 0.2), Color(0.2, 0.2, 0.2), Color(0.2, 0.2, 0.2), 0.1, 2, 0, 0);
    Material m7(Color(0.09, 0.1, 0.1), Color(0.5, 0.7, 0.2), Color(), 5, 2, 0.8, 0.7);
    Material m8(Color(0.09, 0.1, 0.1), Color(0.2, 0.7, 0.5), Color(0.5, 0.5, 0.5), 0.5, 10, 0, 0);



    double box_size = 12;

    //YZ far
    Matrix m = Matrix::TranslateMatrix(-box_size / 2, 0, 0);
    m = Matrix::RotateY(PI / 2) * m;
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));
    //XY top
    m = Matrix::TranslateMatrix(0, 0, box_size / 2);
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1, true));
    //XY bottom
    m = Matrix::TranslateMatrix(0, 0, -box_size / 2);
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));
    //XZ left
    m = Matrix::TranslateMatrix(0, box_size / 2, 0);
    m = Matrix::RotateX(PI / 2) * m;
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));
    //XZ right
    m = Matrix::TranslateMatrix(0, -box_size / 2, 0);
    m = Matrix::RotateX(-PI / 2) * m;
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));

    objects.push_back(new ObjectBox(Vector(0, -2, -box_size / 2 + 1.5), Vector(0, 0, -0.5), 3, m8));

    objects.push_back(new ObjectSphere(Vector(1.5, 1.5, -box_size / 2 + 2), 2, m7));

    double x = 0, y = 0;
    for(x = 0; x < 1; x += 0.2)
        for(y = 0; y < 1; y += 0.2)
        {
            lights.push_back(ObjectLight(Vector(x + 2,-4 + y, 2), Color(0.02, 0.02, 0.02)));
            lights.push_back(ObjectLight(Vector(x + 4,y + 4, 3), Color(0.02, 0.02, 0.02)));
        }


    start_ray_tracing(width(), height());
}

MainWindow::~MainWindow()
{
    for(int i = 0; i < buf_width; i++)
        delete[] buf[i];
    delete[] buf;
    delete ui;
}

#define MAX_DEPTH  5

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
        reflect_ray_color = ray_tracing(intr.reflect_ray, depth_, rays_count, &d);
        reflect_ray_color = reflect_ray_color * exp(-objects[i_object]->m_material.m_beta);
    }

    Color refract_ray_color;
    if (objects[i_object]->m_material.m_refract_amount > 0)
        refract_ray_color = ray_tracing(intr.refract_ray, depth_, rays_count, NULL) * objects[i_object]->m_material.m_refract_amount;

    ret = objects[i_object]->m_material.m_ambient +
          objects[i_object]->m_material.m_diffuse * diffuse * intr.pixel +
          objects[i_object]->m_material.m_specular * specular +
            reflect_ray_color +
            refract_ray_color ;
    //fflush(stdout);
    return ret;
}

void MainWindow::start_ray_tracing(const size_t & width, const size_t & height){
    if (!buf){
        buf_width = width + 1;
        buf = new Color*[buf_width];
        for(int i = 0; i < buf_width; i++)
            buf[i] = new Color[height + 1];
    }
    else
        return;
    double ratio = (double)width / (double)height;
    Vector camera_pos(17, 0, 0);
    double w = 6;
    double h = w / ratio;
    double f = 12;
    Viewport viewport(Vector(f, -w/2, h/2),
                      Vector(f, w/2, h/2),
                      Vector(f, -w/2, -h/2),
                      Vector(f, w/2, -h/2));
    double step_y = w / width;

    for(double y = viewport.m_p1.y, xx = 0; y < viewport.m_p2.y; y += step_y, xx++){
        int rays_count = 0;
        for(double z = viewport.m_p1.z, yy = 0; z > viewport.m_p3.z; z -= step_y, yy++){
            Vector viewport_point(viewport.m_p1.x, y, z);
            Ray first_ray(viewport_point, camera_pos);
            Matrix m = Matrix::RotateY(-0.1);
            first_ray.start_point = camera_pos + Vector(0, 0, -1);
            first_ray.vector = m.mul(first_ray.vector);
            buf[(int)xx][(int)yy] =ray_tracing(first_ray, 0, rays_count, NULL);
        }
        printf("%d/%u rays=%d\n", (int)xx, width, rays_count);
        fflush(stdout);
    }
}

void MainWindow::paintEvent(QPaintEvent *pevent){
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(pevent->rect(), QColor(0, 0, 0));
    for(int x = 0; x < pevent->rect().width(); x++)
        for(int y = 0; y < pevent->rect().height(); y++){
            QPen p = QPen(buf[x][y].convert());
            painter.setPen(p);
            painter.drawPoint(x, y);
        }
    painter.end();
}
