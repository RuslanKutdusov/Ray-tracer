#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "Object.hpp"
#include "Color.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    std::vector<Object*> objects;
    std::vector<ObjectLight> lights;
    bool drawed;
    Color ray_tracing(const Ray & ray, const int & depth, int & rays_count, double *distance);
protected:
    void paintEvent(QPaintEvent * pevent);
};

#endif // MAINWINDOW_H
