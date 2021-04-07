#define F_VECTOR_SIZE 128
#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QtGui>
#include <QWidget>
#include <QColor>
#include "r2geom.h"

const int MAX_POINTS = 100;

// Catch/drag state
const int CATCH_NONE = 0;
const int CATCH_SHOW = 1;
const int CATCH_DRAG = 2;

struct handdata{
    int Number = -1;
    int value_vector[F_VECTOR_SIZE];
};

class DrawArea: public QWidget
{
    Q_OBJECT

private:

    friend class MainWindow;

    int num_counts[10];
    QPainter qp;
    QPen pen;
    bool input_is_on; // Tracking user-s drawing process
    QColor bgColor;   // Background color
    int xSize;        // Window width
    int ySize;        // Window height
    int answer_size = F_VECTOR_SIZE;
    int neighbours_num; // Number of neighbours in classification (1)

    // Mathematical coordinate system in window
    double xmin, xmax, ymin, ymax;
    double xCoeff, yCoeff;

    // Correspondence between pixel and mathematical coord. systems
    QPoint map(const R2Point& p) const;
    R2Point invMap(const QPoint& q) const;

    QPointF mapF(const R2Point& p) const;
    R2Point invMap(const QPointF& q) const;

    // The array of points defined by mouse clicks
    int numPoints;
    bool CUSTOM_INPUT;
    int CUSTOM_NUMBER;
    R2Point q[MAX_POINTS + 1];  // Nodes of interpolation
    R2Point q_stretched[MAX_POINTS +1]; // Unification
    R2Point q_azimut[MAX_POINTS +1];  // Feature extraction
    int azimut[MAX_POINTS +1];
    Qt::MouseButton mouseButtons[MAX_POINTS + 1];
    int n;
    int answer[128];  // Feature-vector interpolated to Feature-Vector size
    bool customing_symbols;     // Custom-mode for user to form his own handwriting
    // Catch/drag a point by mouse
    int catchState;             // CATCH_NONE/SHOW/DRAG
    int catchPointIdx;          // Index of a point catched
    R2Point catchInitialPos;    // Initial position of catched point

public:

    DrawArea(QWidget *parent = 0);

    void initializeMap();
    void drawCoordSystem(QPainter*);
    void drawInput(QPainter* painter);
    void redraw_input_stretched_and_azimut(QPainter* painter);
    void drawPoints(QPainter*);
    void drawControlPoints(QPainter*);

    void parce_handwr_data_from_txt();
    void clear();
    void onDraw();  // Called when a user presses the "Draw" button
    int findPoint(const R2Point& pos) const;

    ~DrawArea(){
     //   free(answer);
    }

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void resizeEvent(QResizeEvent *event);
};


#endif
