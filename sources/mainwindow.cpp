#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QScreen>
//#include <iostream>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    main (new QWidget),
    myGrid (new Grid(main)),
    vBox (new QVBoxLayout),
    buttLayout (new QHBoxLayout),
    hBox (new QHBoxLayout),
    controlsBox (new QVBoxLayout),
    fileDropdown (new QComboBox),
    fullScreenButt (new QPushButton),
    minimizeButt (new QPushButton),
    brightnessSpin (new QSpinBox),
    contrastSpin (new QSpinBox),
    winLowSpin (new QSpinBox),
    winHighSpin (new QSpinBox),
    windowSlider (new QRangeSlider),
    resetAllButt (new QPushButton),
    resetBrightnessButt (new QPushButton),
    resetContrastButt (new QPushButton),
    resetWindowButt (new QPushButton)
{
    ui->setupUi(this);
    setCentralWidget(main);
    main->setLayout(vBox);
    vBox->addLayout(buttLayout, Qt::AlignTop);
    vBox->addLayout(hBox, Qt::AlignBottom);


    fileDropdown->addItem("File");
    fileDropdown->addItem("Load File");
    fileDropdown->addItem("Load Folder");
    fileDropdown->addItem("Save Selected");

    connect(fileDropdown, &QComboBox::currentTextChanged, [this](const QString &val){
        std::string doWhat = val.toStdString();

        if (doWhat == "Load File"){
            load_file();
        } else if (doWhat == "Load Folder"){
            load_folder();
        } else if (doWhat == "Save Selected"){
            save_file();
        }

        fileDropdown->setCurrentText("File");
    });

    buttLayout->addWidget(fileDropdown);
    buttLayout->addWidget(fullScreenButt);
    buttLayout->addWidget(minimizeButt);

    brightnessSpin->setRange(-500, 500);
    brightnessSpin->setValue(0);
    brightnessSpin->setSingleStep(10);
    QObject::connect(brightnessSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        set_brightness(value);
    });

    contrastSpin->setRange(10, 300);
    contrastSpin->setValue(100);
    contrastSpin->setSingleStep(5);
    QObject::connect(contrastSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        set_contrast(value);
    });

    int min = -500;
    int max = 500;
    int lo = 0;
    int hi = 255;

    winLowSpin->setRange(min, max);
    winLowSpin->setSingleStep(1);

    winHighSpin->setRange(min, max);
    winHighSpin->setSingleStep(1);


    windowSlider->setRange(min, max);
    windowSlider->setLowValue(lo);
    windowSlider->setHighValue(hi);
    /*
    winLowSpin->setRange(-10, 10);
    winLowSpin->setSingleStep(5);

    winHighSpin->setRange(-10, 10);
    winHighSpin->setSingleStep(5);


    windowSlider->setRange(-10, 10);
    windowSlider->setLowValue(5);
    windowSlider->setHighValue(7);
*/
    winLowSpin->setValue(windowSlider->lowValue());
    winHighSpin->setValue(windowSlider->highValue());

    connect(winLowSpin, &QSpinBox::valueChanged, windowSlider, &QRangeSlider::setLowValue);
    connect(winHighSpin, &QSpinBox::valueChanged, windowSlider, &QRangeSlider::setHighValue);

    connect(windowSlider, &QRangeSlider::lowValueChange, winLowSpin, &QSpinBox::setValue);
    connect(windowSlider, &QRangeSlider::highValueChange, winHighSpin, &QSpinBox::setValue);

    connect(windowSlider, &QRangeSlider::rangeChange, winLowSpin, &QSpinBox::setRange);
    connect(windowSlider, &QRangeSlider::rangeChange, winHighSpin, &QSpinBox::setRange);

    QObject::connect(winLowSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        set_min(value);
    });

    QObject::connect(winHighSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        set_max(value);
    });

    resetAllButt->setText("Reset All");
    connect(resetAllButt, &QPushButton::clicked, this, [=]() {
        reset_brightness();
        reset_contrast();
        reset_window();
    });

    resetBrightnessButt->setText("Reset BrightnessButt");
    connect(resetBrightnessButt, &QPushButton::clicked, this, [=]() {
        reset_brightness();
    });

    resetContrastButt->setText("Reset Contrast");
    connect(resetContrastButt, &QPushButton::clicked, this, [=]() {
        reset_contrast();
    });

    resetWindowButt->setText("Reset Window");
    connect(resetWindowButt, &QPushButton::clicked, this, [=]() {
        reset_window();
    });

    controlsBox->addWidget(resetAllButt);

    controlsBox->addWidget(resetBrightnessButt);
    controlsBox->addWidget(brightnessSpin);

    controlsBox->addWidget(resetContrastButt);
    controlsBox->addWidget(contrastSpin);

    controlsBox->addWidget(resetWindowButt);
    controlsBox->addWidget(winLowSpin);
    controlsBox->addWidget(winHighSpin);
    controlsBox->addWidget(windowSlider);

    isFullScreen = true;
    isMaximized = false;

    int winWidth = this->width();   // Get the width of the main window
    int winHeight = this->height();
    //std::cerr << "W Width: " << winWidth << " WHeight: " << winHeight << std::endl;

    QSize currentSize = main->size(); // Gets the current size as a QSize object
    int currentWidth = main->width(); // Gets the current width
    int currentHeight = main->height();
    //std::cerr << "Width: " << currentWidth << " Height: " << currentHeight << std::endl;

      // Start the Python interpreter

    myGrid->show();
    hBox->addWidget(myGrid);
    hBox->addLayout(controlsBox);

    myGrid->setMaximumSize(int(0.8 * currentWidth), int(0.9 * currentHeight));
    fileDropdown->setMaximumHeight(int(0.09 * currentHeight));
    fullScreenButt->setMaximumHeight(int(0.09 * currentHeight));
    minimizeButt->setMaximumHeight(int(0.09 * currentHeight));

    fullScreenButt->setText("Exit FScrn");
    connect(fullScreenButt, &QPushButton::clicked, this, [=]() {
        fullscreen();
    });

    minimizeButt->setText("Minimize");
    connect(minimizeButt, &QPushButton::clicked, this, [=]() {
        minimize();
    });

    showFullScreen();
}


void MainWindow::load_file(){
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,                // Parent widget (nullptr if no parent)
        "Open DICOM File",      // Dialog title
        "",                     // Initial directory (empty means default)
        "DICOM Files (*.dcm *.ima *.hdr *.dicom)"  // Filter for DICOM files
        );

    if (!filePath.isEmpty()) {
        std::string path = filePath.toStdString();
        myGrid->read_dicom(path, "file");
    }
}

void MainWindow::load_folder(){
    QString directoryPath = QFileDialog::getExistingDirectory(
        nullptr,
        "Select Directory",
        "",
        QFileDialog::ShowDirsOnly
        );

    if (!directoryPath.isEmpty()) {
        std::string path = directoryPath.toStdString();
        myGrid->read_dicom(path, "dir");
    }
}

void MainWindow::save_file(){
    QString path = "";
    std::string savePath = "";
    if (myGrid->tabList.size() == 1){
        path = QFileDialog::getSaveFileName(nullptr, tr("Save File"),
                                                        QDir::homePath(),
                                                        tr("Images (*.png *.jpg)"));
        savePath = path.toStdString();
        cv::imwrite(savePath, myGrid->tabList[0]->outMat);
    } else if (myGrid->tabList.size() > 1){
        path = QFileDialog::getExistingDirectory(
            nullptr,
            "Select Directory",
            "",
            QFileDialog::ShowDirsOnly
            );

        if (path.isEmpty()) {
            return;
        }
        std::string savePath = path.toStdString();
        int i = 0;
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                std::string tempPath = savePath + "/dcm-viewer-" + std::to_string(i) + ".png";
                cv::imwrite(tempPath,tab->outMat);
                i++;
            }
        }
    }
}

void MainWindow::set_brightness(int val){
    if (myGrid->tabList.size() > 0){
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                tab->change_brightness(val);
            }
        }
    }
}

void MainWindow::set_contrast(int val){
    if (myGrid->tabList.size() > 0){
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                tab->change_contrast(val);
            }
        }
    }
}

void MainWindow::set_min(int val){
    if (myGrid->tabList.size() > 0){
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                tab->change_min(val);
            }
        }
    }
}

void MainWindow::set_max(int val){
    if (myGrid->tabList.size() > 0){
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                tab->change_max(val);
            }
        }
    }
}

void MainWindow::reset_brightness(){
    brightnessSpin->setValue(0);
    if (myGrid->tabList.size() > 0){
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                tab->currentBrightness = 0;
                tab->update_mat();
            }
        }
    }
}

void MainWindow::reset_contrast(){
    contrastSpin->setValue(100);
    if (myGrid->tabList.size() > 0){
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                tab->currentAlpha = 1;
                tab->update_mat();
            }
        }
    }
}

void MainWindow::reset_window(){
    windowSlider->setLowValue(0);
    windowSlider->setHighValue(255);
    winLowSpin->setValue(windowSlider->lowValue());
    winHighSpin->setValue(windowSlider->highValue());
    if (myGrid->tabList.size() > 0){
        for (Tab* tab : myGrid->tabList){
            if (tab->selected){
                tab->currentMax = tab->ogMax;
                tab->currentMin = tab->ogMin;
                tab->update_mat();
            }
        }
    }
}


void MainWindow::fullscreen(){
    if (isFullScreen){
        fullScreenButt->setText("Fullscreen");
        showMaximized();
        main->resize(this->size());
    } else {
        fullScreenButt->setText("Exit FScrn");
        showFullScreen();
    }
    isFullScreen = !isFullScreen;
}

void MainWindow::minimize(){
    showMinimized();
    minimizeButt->setText("Minimize");
}

void MainWindow::resizeEvent(QResizeEvent *event){
    QMainWindow::resizeEvent(event); // Ensure base class handles the event

    int winWidth = this->width();
    int winHeight = this->height();
    //std::cerr << "W Width: " << winWidth << " WHeight: " << winHeight << std::endl;

    // Optionally, adjust the maximum size of the central widget
    main->setMaximumSize(winWidth, winHeight);
    main->resize(this->size());

    QSize currentSize = main->size();
    int currentWidth = currentSize.width();
    int currentHeight = currentSize.height();
    //std::cerr << "Width: " << currentWidth << " Height: " << currentHeight << std::endl;

    if (myGrid != nullptr){
        myGrid->setFixedSize(int(0.8 * currentWidth), int(0.9 * currentHeight));
        //std::cerr << "Grid Width: " << int(0.8 * currentWidth) << " Grid Height: " << int(0.9 * currentHeight) << std::endl;
    }
    //addButt->setMaximumSize(int(0.33 * currentWidth), int(0.09 * currentHeight));
    //fullScreenButt->setMaximumSize(int(0.33 * currentWidth), int(0.09 * currentHeight));
    //minimizeButt->setMaximumSize(int(0.33 * currentWidth), int(0.09 * currentHeight));
}

MainWindow::~MainWindow()
{
    delete ui;
}
