#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "grid.h"
#include "qrangeslider.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QWidget* main;
    Grid* myGrid;
    QVBoxLayout* vBox;
    QHBoxLayout* buttLayout;
    QHBoxLayout* hBox;
    QVBoxLayout* controlsBox;
    QComboBox* fileDropdown;
    QPushButton* fullScreenButt;
    QPushButton* minimizeButt;
    QSpinBox* brightnessSpin;
    QSpinBox* contrastSpin;
    QSpinBox* winLowSpin;
    QSpinBox* winHighSpin;
    QRangeSlider* windowSlider;
    QPushButton* resetAllButt;
    QPushButton* resetBrightnessButt;
    QPushButton* resetContrastButt;
    QPushButton* resetWindowButt;

    bool isFullScreen;
    bool isMaximized;

    void load_file();
    void load_folder();
    void save_file();

    void set_contrast(int);
    void set_brightness(int);
    void set_min(int);
    void set_max(int);
    void reset_brightness();
    void reset_contrast();

    void reset_window();
    void fullscreen();
    void minimize();
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
