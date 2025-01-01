#ifndef TAB_H
#define TAB_H

#include <opencv2/opencv.hpp>
#include <vector>

#include <QResizeEvent>
#include <QSize>
#include <QWidget>
#include <QPixmap>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>

class Tab : public QWidget
{
    Q_OBJECT
public:
    explicit Tab(QWidget* parent = nullptr,
                 int num = 0,
                 std::vector<cv::Mat> m_frames = {},
                 std::string m_study_uid = "",
                 std::string m_series_uid = "",
                 double m_orientation[6] = nullptr,
                 double m_row_spacing = 0.0,
                 double m_col_spacing = 0.0,
                 double m_thickness = 0.0);

    QPixmap* myPix;
    QPixmap* scaledPix;
    QGraphicsPixmapItem* qGPix;
    QGraphicsView* gViewer;
    QGraphicsScene* scene;

    QWidget *secondaryWindow;
    QVBoxLayout *secondaryLayout;

    QLabel* pixLabel;
    QVBoxLayout* layout;
    QHBoxLayout* buttLayout;
    QSpinBox* indexSpin;
    QSpinBox* zoomSpin;
    QSpinBox* rotateSpin;
    QPushButton* syncButt;
    QPushButton* selectButt;
    QPushButton* resetZoomButt;
    QPushButton* resetRotateButt;
    QPushButton* closeButt;
    QPushButton* focusButt;
    QPushButton* popButt;

    bool selected, isPopped;
    int id;
    QSize tabSize;

    std::vector<cv::Mat> frames;
    cv::Mat currentMat;
    cv::Mat outMat;
    int currentIndex;
    std::string study_uid, series_uid;
    double ogMin, ogMax, currentMin, currentMax;
    double orientation[6];
    double row_spacing, col_spacing, thickness;
    int currentBrightness;
    float currentAlpha;
    qreal ogScale;

    void select_toggle();
    void set_index(int);
    //void close_pop(QWidget*);
    //void change_contrast(cv::Mat&, cv::Mat&, int);
    //void change_brightness(cv::Mat&, cv::Mat&, int);
    void change_contrast(int);
    void change_brightness(int);
    void change_min(int);
    void change_max(int);
    void change_zoom(int);
    void change_rotate(int);
    void update_mat();
    void matToPixmap(cv::Mat&, QPixmap&, double, double, QSize);
    void resizeEvent(QResizeEvent *event) override;

};

#endif // TAB_H
