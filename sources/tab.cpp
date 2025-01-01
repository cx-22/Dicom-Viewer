#include "tab.h"
#include <iostream>

Tab::Tab(QWidget* parent,
         int num,
         std::vector<cv::Mat> m_frames,
         std::string m_study_uid,
         std::string m_series_uid,
         double m_orientation[6],
         double m_row_spacing,
         double m_col_spacing,
         double m_thickness)
    : QWidget(parent),
    myPix(new QPixmap()),
    scaledPix(new QPixmap()),
    qGPix(new QGraphicsPixmapItem()),
    gViewer(new QGraphicsView()),
    scene(new QGraphicsScene()),

    secondaryWindow(new QWidget()),
    secondaryLayout(new QVBoxLayout()),

    pixLabel(new QLabel(this)),
    layout(new QVBoxLayout(this)),
    buttLayout(new QHBoxLayout),
    indexSpin(new QSpinBox),
    zoomSpin(new QSpinBox),
    rotateSpin(new QSpinBox),

    syncButt(new QPushButton),
    selectButt(new QPushButton),
    resetZoomButt(new QPushButton),
    resetRotateButt(new QPushButton),
    closeButt(new QPushButton),
    focusButt(new QPushButton),
    popButt(new QPushButton)
{
    secondaryWindow->setLayout(secondaryLayout);
    secondaryWindow->setWindowTitle("Secondary Window");

    id = num;
    selected = false;
    isPopped = false;
    currentBrightness = 0;
    currentAlpha = 1;

    frames = m_frames;
    currentIndex = 0;
    study_uid = m_study_uid;
    series_uid = m_series_uid;
    for (int i = 0; i < 6; ++i) {
        orientation[i] = m_orientation[i];
    }
    row_spacing  = m_row_spacing;
    col_spacing = m_col_spacing;
    thickness =  m_thickness;

    tabSize = QSize(400, 400);

    buttLayout->addWidget(indexSpin);

    buttLayout->addWidget(resetZoomButt);
    buttLayout->addWidget(zoomSpin);
    buttLayout->addWidget(resetRotateButt);
    buttLayout->addWidget(rotateSpin);

    buttLayout->addWidget(syncButt);
    buttLayout->addWidget(selectButt);
    buttLayout->addWidget(focusButt);
    buttLayout->addWidget(closeButt);
    buttLayout->addWidget(popButt);
    layout->addLayout(buttLayout);

    indexSpin->setRange(0, frames.size() - 1);
    indexSpin->setValue(0);
    indexSpin->setSingleStep(1);

    zoomSpin->setRange(0, 1000);
    zoomSpin->setValue(100);
    zoomSpin->setSingleStep(10);

    rotateSpin->setRange(-720, 720);
    rotateSpin->setValue(0);
    rotateSpin->setSingleStep(1);

    syncButt->setText("Sync");
    selectButt->setText("Select");
    resetZoomButt->setText("Zoom %");
    resetRotateButt->setText("Rotate °");
    closeButt->setText("Close");
    focusButt->setText("Focus");
    popButt->setText("Pop Out");

    int minHeight = 30;

    indexSpin->setMinimumHeight(minHeight);
    zoomSpin->setMinimumHeight(minHeight);
    rotateSpin->setMinimumHeight(minHeight);
    syncButt->setMinimumHeight(minHeight);
    selectButt->setMinimumHeight(minHeight);
    resetZoomButt->setMinimumHeight(minHeight);
    resetRotateButt->setMinimumHeight(minHeight);
    closeButt->setMinimumHeight(minHeight);
    focusButt->setMinimumHeight(minHeight);
    popButt->setMinimumHeight(minHeight);

    gViewer->setDragMode(QGraphicsView::ScrollHandDrag);
    gViewer->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    gViewer->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    gViewer->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //layout->addWidget(gViewer, 0,Qt::AlignHCenter);
    //layout->addWidget(pixLabel, 0,Qt::AlignHCenter);
    //pixLabel->setScaledContents(false);

    QObject::connect(indexSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        set_index(value);
    });

    QObject::connect(zoomSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        change_zoom(value);
    });

    QObject::connect(rotateSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        change_rotate(value);
    });

    connect(resetZoomButt, &QPushButton::clicked, this, [=]() {
        zoomSpin->setValue(100);
    });

    connect(resetRotateButt, &QPushButton::clicked, this, [=]() {
        rotateSpin->setValue(0);
    });

    connect(selectButt, &QPushButton::clicked, this, [=]() {
        select_toggle();
    });


    layout->addWidget(gViewer);
    gViewer->show();

    currentMat = frames[0].clone();
    outMat = frames[0].clone();
    std::cout << "Mat width: " << currentMat.rows << std::endl;
    cv::minMaxLoc(currentMat, &ogMin, &ogMax);
    std::cout << "ogMIN: " << ogMin << "  ogMAX: " << ogMax << std::endl;
    matToPixmap(currentMat, *myPix, ogMin, ogMax, tabSize);
    currentMin = ogMin;
    currentMax = ogMax;

    qGPix->setPixmap(*myPix);
    qGPix->setTransformOriginPoint(myPix->width() / 2.0, myPix->height() / 2.0);

    QSizeF pmSize = qGPix->pixmap().size();
    QSizeF viewSize = gViewer->size();

    qreal xScale = pmSize.width() / viewSize.width();
    qreal yScale = pmSize.height() / viewSize.height();

    ogScale = qMax(xScale, yScale);
    gViewer->setTransform(QTransform().scale(1 / ogScale, 1 / ogScale));
    scene->addItem(qGPix);
    scene->setSceneRect(qGPix->boundingRect());
    gViewer->setScene(scene);


    //delete scaledPix;
    //scaledPix = new QPixmap(myPix->scaled(tabSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //pixLabel->setPixmap(*scaledPix);
}

void Tab::select_toggle(){
    if (selected){
        selectButt->setText("Select");
    } else {
        selectButt->setText("Unelect");
    }
    selected = !selected;
}

void Tab::set_index(int index){
    currentIndex = index;
    currentMat = frames[index].clone();
    update_mat();

    //matToPixmap(currentMat, *myPix, currentMin, currentMax, scaledPix->size());
    //delete scaledPix;
    //scaledPix = new QPixmap(myPix->scaled(scaledPix->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //pixLabel->setPixmap(*scaledPix);
}


//void Tab::change_brightness(cv::Mat& in, cv::Mat& out, int val){
void Tab::change_brightness(int val){
    currentBrightness = val;
    /*
    cv::Mat out;
    currentMat.convertTo(out, -1, 1, val); // Apply brightness change
    cv::minMaxLoc(out, &currentMin, &currentMax);
    matToPixmap(out, *myPix, currentMin, currentMax, scaledPix->size());
    */

    update_mat();
}

//void Tab::change_contrast(cv::Mat& in, cv::Mat& out, int val){
void Tab::change_contrast(int val){
    currentAlpha = static_cast<float>(val) / 100;
    update_mat();
}

void Tab::change_min(int val){
    currentMin = static_cast<double>(val);
    update_mat();
}

void Tab::change_max(int val){
    currentMax = static_cast<double>(val);
    update_mat();
}

void Tab::change_zoom(int val){
    qreal factor = (static_cast<float>(val) / 100) / ogScale;
    gViewer->setTransform(QTransform().scale(factor, factor));
}

void Tab::change_rotate(int val){
    qGPix->setRotation(val);
}

void Tab::update_mat(){
    outMat = currentMat.clone();
    if (currentAlpha != 1){
        outMat.convertTo(outMat, -1, currentAlpha, 0);
    }
    if (currentBrightness != 0){
        outMat.convertTo(outMat, -1, 1, currentBrightness);
    }
    //std::cout << "currentMIN: " << currentMin << "  currentMAX: " << currentMax << std::endl;
    //cv::minMaxLoc(out, &currentMin, &currentMax);
    //std::cout << "currentMIN: " << currentMin << "  currentMAX: " << currentMax << std::endl;

    matToPixmap(outMat, *myPix, currentMin, currentMax, scaledPix->size());
    //matToPixmap(out, *myPix, ogMin, ogMax, scaledPix->size());

}


void Tab::matToPixmap(cv::Mat& in, QPixmap& pixmap, double min, double max, QSize size){
    //if (in.depth() == CV_16S) {
        //std::cout << "inMIN: " << min << "  inMAX: " << max << std::endl;
        in.convertTo(in, CV_8U, 255.0 / (max - min), -min * 255.0 / (max - min));
    //}

    if (in.channels() == 1) {
        cv::cvtColor(in, in, cv::COLOR_GRAY2RGB);
    }
    QImage q_image = QImage(in.data, in.cols, in.rows, in.step, QImage::Format_RGB888);
    delete myPix;
    myPix = new QPixmap(QPixmap::fromImage(q_image));
    //QSize pixmapSize = pixmap.size();
    //std::cout << "Pixmap size:" << pixmapSize.width() << "x" << pixmapSize.height() << std::endl;


    qGPix->setPixmap(*myPix);
    scene->addItem(qGPix);
    scene->setSceneRect(qGPix->boundingRect());
    //delete scaledPix;
    //scaledPix = new QPixmap(myPix->scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //pixLabel->setPixmap(*scaledPix);

}

void Tab::resizeEvent(QResizeEvent *event) {
    //double aspectRatio = static_cast<double>(currentMat.cols) / currentMat.rows;

    //int newWidth = event->size().width();
    //int newHeight = event->size().height();

    //if (newWidth / aspectRatio <= newHeight) {
    //    newHeight = static_cast<int>(newWidth / aspectRatio);
    //} else {
    //    newWidth = static_cast<int>(newHeight * aspectRatio);
    //}

    //gViewer->resize(newWidth, newHeight);

    QSizeF pmSize = qGPix->pixmap().size();
    QSizeF viewSize = gViewer->size();

    qreal xScale = pmSize.width() / viewSize.width();
    qreal yScale = pmSize.height() / viewSize.height();

    ogScale = qMin(xScale, yScale);
    gViewer->setTransform(QTransform().scale(1 / ogScale, 1 / ogScale));



    //delete scaledPix;
    //scaledPix = new QPixmap(myPix->scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //pixLabel->setPixmap(*scaledPix);
}

