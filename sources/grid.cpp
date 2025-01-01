#include "grid.h"
#include <QList>
//#include <iostream>

/*
    MyGrid();
    ~MyGrid();
    std::vector<Tab*> tabList;
    void add_tab();
    void close_tab();
    void focus_tab();
    void fullscreen_tab();
    void pop_out_tab();
    void pop_in_tab();
*/

namespace py = pybind11;


Grid::Grid(QWidget* parent, int w, int h)
    : QWidget(parent),
    grid(new QGridLayout(this))
{
    width = w;
    height = h;
    counter = 0;
    grid->setSpacing(5);
    grid->setContentsMargins(0, 0, 0, 0);
    setLayout(grid);
}










void Grid::read_dicom(std::string filePath, std::string fileType) {

    // Import the 'sys' module and append the current directory to the path
    py::module_ sys = py::module_::import("sys");
    sys.attr("path").attr("append")("C:/Users/nicka/Documents/Qt-projects/Dicom_Viewer");
    py::module_ dicom_reader = py::module_::import("pydy");

    py::object read_dicom_func = dicom_reader.attr("read_dicom");
    py::tuple result = read_dicom_func(filePath, fileType).cast<py::tuple>();
    py::list slice_list = result[0].cast<py::array>();

    // Extract the metadata
    std::string study_uid = result[1].cast<std::string>();
    std::string series_uid = result[2].cast<std::string>();
    py::array orientation_array = result[3].cast<py::array>();
    double row_spacing = result[4].cast<double>();
    double col_spacing = result[5].cast<double>();
    double thickness = result[6].cast<double>();
    int num_frames = result[7].cast<int>();

    double orientation[6];

    auto ptr = orientation_array.mutable_data();  // Pointer to the raw data
    double* orientation_data = static_cast<double*>(ptr);
    memcpy(orientation, orientation_data, 6 * sizeof(double));

    int type, rows, cols;

    std::vector<cv::Mat> frames;

    for (int i = 0; i < num_frames; i++){
        py::array slice = slice_list[i].cast<py::array>();
        py::buffer_info info = slice.request();

        if (i == 0){
            rows = info.shape[0];
            cols = info.shape[1];
            if (info.format == py::format_descriptor<int>::format()) {
                type = CV_32S;
            } else if (info.format == py::format_descriptor<float>::format()) {
                type = CV_32F;
            } else if (info.format == py::format_descriptor<double>::format()) {
                type = CV_64F;
            } else if (info.format == py::format_descriptor<unsigned char>::format()) {
                type = CV_8U;
            } else if (info.format == py::format_descriptor<int8_t>::format()) {
                type = CV_8S;
            } else if (info.format == py::format_descriptor<uint8_t>::format()) {
                type = CV_8U;
            } else if (info.format == "h") {
                type = CV_16S;
            } else if (info.format == "H") {
                type = CV_16U;
            } else {
                throw std::runtime_error("Unsupported NumPy array format: " + std::string(info.format));
            }
        }
        frames.push_back(cv::Mat(rows, cols, type, const_cast<void*>(info.ptr)).clone());
    }

    Tab* tab = new Tab(this,
                        counter,
                        frames,
                        study_uid,
                        series_uid,
                        orientation,
                        row_spacing,
                        col_spacing,
                        thickness);
    add_tab(tab);
}

void Grid::add_tab(Tab* temp){
    //std::cout << "just made: " << temp->id << std::endl;
    QObject::connect(temp->closeButt, &QPushButton::clicked, [temp, this]() {
        close_tab(temp);
    });

    QObject::connect(temp->focusButt, &QPushButton::clicked, [temp, this]() {
        focus_tab(temp);
    });

    QObject::connect(temp->popButt, &QPushButton::clicked, [temp, this]() {
        pop_out_tab(temp);
    });

    clear_grid();
    tabList.push_back(temp);
    draw_grid();
    counter++;
}

void Grid::close_tab(Tab* tab){
    //std::cout << "this: " << tab->id << std::endl;
    for (size_t i = 0; i < tabList.size(); i++){
        //std::cout << "test: " << tabList[i]->id << std::endl;
        if ( (tabList[i] != nullptr) && (tabList[i]->id == tab->id) ){
            grid->removeWidget(tab);
            delete tabList[i];
            tabList.erase(tabList.begin() + i);
            break;
        }
    }
    clear_grid();
    draw_grid();
}
void Grid::focus_tab(Tab* tab){
    clear_grid();
    grid->addWidget(tab, 0, 0);
    tab->show();
    tab->focusButt->setText("Unfocus");
    QObject::connect(tab->focusButt, &QPushButton::clicked, [&]() {
        draw_grid();
        focusedTab = tab;
        unfocus();
    });
}

void Grid::unfocus(){
    Tab* tab = focusedTab;
    tab->focusButt->setText("Focus");
    QObject::connect(tab->focusButt, &QPushButton::clicked, [tab, this]() {
        focus_tab(tab);
    });
    focusedTab = nullptr;
    draw_grid();
}

void Grid::pop_out_tab(Tab* tab){
    if (tab->isPopped){
        tab->secondaryLayout->removeWidget(tab);
        clear_grid();
        draw_grid();
        tab->popButt->setText("Pop Out");
        tab->secondaryWindow->hide();
    } else {
        tab->secondaryLayout->addWidget(tab);
        tab->popButt->setText("Pop In");
        tab->secondaryWindow->show();
    }

    tab->isPopped = !tab->isPopped;
}

/*
void Grid::pop_out_tab(Tab* tab){

}

void Grid::pop_in_tab(Tab* tab){

}*/

void Grid::clear_grid(){
    int removedCount = 0;
    for (int i = 0; i < tabList.size(); i++) {
        tabList[i]->hide();
        grid->removeWidget(tabList[i]);
        removedCount++;
    }
}

void Grid::draw_grid(){
    int num = tabList.size();
    int i, j, x = 0;
    if (num < 3 && num > 0){
        for (i = 0; i < num; i++){
            //tabList[i]->tabWidth = width / num;
            //tabList[i]->tabHeight = height;
            tabList[i]->tabSize = QSize(width / num, height);
            grid->addWidget(tabList[i], 0, i);
            tabList[i]->show();
        }
    } else if (num > 0){
        int temp = (num % 2 == 0) ? num : (num + 1);
        for (i = 0; i < 2; i++){
            for (j = 0; j < (temp / 2); j++){
                if (x < num){
                    //tabList[x]->tabWidth = width / (temp / 2);
                    //tabList[x]->tabHeight = height / 2;
                    tabList[x]->tabSize = QSize(width / (temp / 2), height / 2);
                    grid->addWidget(tabList[x], i, j);
                    tabList[x]->show();
                    x++;
                }
            }
        }
    }
    for (int i = 0; i < tabList.size(); i++){
        tabList[i]->resize(tabList[i]->width(), tabList[i]->height());
    }
}

void Grid::update_grid(int w, int h){
    width = w;
    height = h;
    clear_grid();
    draw_grid();
}

//void MyGrid::fullscreen_tab(){}
//void MyGrid::pop_out_tab(){}
//void MyGrid::pop_in_tab(){}

Grid::~Grid(){
    for (size_t i = 0; i < tabList.size(); i++){
        if (tabList[i] != nullptr){
            delete tabList[i];
        }
    }
    delete grid;
}
