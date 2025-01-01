#ifndef GRID_H
#define GRID_H

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <opencv2/opencv.hpp>

#include <QWidget>
#include <QGridLayout>
#include <vector>
#include <QApplication>
#include "tab.h"


class Grid : public QWidget
{
    Q_OBJECT
public:
    explicit Grid(QWidget* parent = nullptr, int w = 400, int h = 400);
    ~Grid();
    std::vector<Tab*> tabList;
    Tab* focusedTab;
    QGridLayout* grid;
    int counter;
    void add_tab(Tab*);
    void close_tab(Tab*);
    void focus_tab(Tab*);
    void unfocus();
    void pop_out_tab(Tab*);
    void draw_grid();
    void clear_grid();
    void update_grid(int, int);
    int width;
    int height;

    void read_dicom(std::string, std::string);
};

#endif // MYGRID_H
