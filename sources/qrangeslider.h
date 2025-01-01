#ifndef Q_RANGE_SLIDER_H
#define Q_RANGE_SLIDER_H

#include <QWidget>

class QRangeSlider : public QWidget
{

    Q_OBJECT

public:
    QRangeSlider(QWidget *parent = nullptr);
    ~QRangeSlider();
    int minimum() const;
    int maximum() const;
    int lowValue() const;
    int highValue() const;
    unsigned int step() const;
    void setStep(const unsigned int step);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void setOrientation(Qt::Orientation orientation);

public slots:
    void setMinimum(const int minimum);
    void setMaximum(const int maximum);
    void setLowValue(const int lowValue);
    void setHighValue(const int highValue);
    void setRange(const int minimum, const int maximum);

signals:
    void minimumChange(int minimum);
    void maximumChange(int maximum);
    void lowValueChange(int lowValue);
    void highValueChange(int highValue);
    void rangeChange(int minimum, int maximum);

private:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *event) override;

    /* Minimum range value */
    int m_minimum = 0;

    /* Maximum range value */
    int m_maximum = 100;

    /* Step value */
    unsigned int m_step = 1;

    /* Value of low handle */
    int m_lowValue;

    /* Value of high handle */
    int m_highValue;

    /* Value of the last mouse click */
    int m_lastMouseValue = -1;

    /* Painter constants */
    /* Slider height in pixels */
    static constexpr unsigned int SLIDER_HEIGHT = 5;
    /* Slider handle size in pixels */
    static constexpr unsigned int HANDLE_SIZE = 13;
    /* Left and Right padding in pixels */
    static constexpr unsigned int PADDING = 1;
};

#endif // Q_RANGE_SLIDER_H
