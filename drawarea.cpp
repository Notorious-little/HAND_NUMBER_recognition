#include <QtGui>
#include <cassert>
#include <cmath>
#include "drawarea.h"
#include "mainwindow.h"
#include "r2geom.h"
#include <iostream>
#include <fstream>


using namespace std;

extern const char EOL = '\n';
static int CATCH_DISTANCE = 4;  // In pixels

int handdata_size = 1000;
handdata* handwritten_nums = new handdata[handdata_size];
int* errors = new int[handdata_size];

DrawArea::DrawArea(QWidget *parent):
    QWidget(parent),
    pen(Qt::black, 2, Qt::SolidLine),
    input_is_on(false),
    bgColor(Qt::lightGray),
    xSize(0),
    ySize(0),
    xmin(-8.),
    xmax(8.),
    ymin(-6.),
    ymax(6.),
    CUSTOM_INPUT(false),
    CUSTOM_NUMBER(5),
    n(0),
    catchState(CATCH_NONE),
    catchPointIdx(-1),
    catchInitialPos()
{
    parce_handwr_data_from_txt();
    setAttribute(Qt::WA_StaticContents); // for optimizing painting events
    setMouseTracking(true);
    drawArea = this;
}

void DrawArea::parce_handwr_data_from_txt(){

    std::vector<int> tmp(128, 0);
    std::string line;
    char c;
    int int_c = 0, i = 0;
    int cur_val(0);
    int NUMBER(0);

    while (NUMBER < 10){

      std::string num_str = std::to_string(NUMBER);
      std::string file_path = "./data/HANDWRITTEN_DATA_" +
              num_str + ".txt";
      std::ifstream in(file_path);

       if (in.is_open()) {

         while ( !in.eof() ){
/*
           if (handdata_size < i-1){

               handdata* handwritten_nums_tmp = new handdata[handdata_size];
               for(int k = 0; k < handdata_size; ++k){
                   handwritten_nums_tmp[k] = handwritten_nums[k];
               }

               delete[] handwritten_nums ;
               handwritten_nums = nullptr;

               handdata_size *= 2;
               handwritten_nums = new handdata[handdata_size];

               for(int k = 0; k < handdata_size/2; ++k){
                  handwritten_nums[k] = handwritten_nums_tmp[k];
               }
           }
*/
           for(int j = 0; j < F_VECTOR_SIZE; ++j){
               in >> cur_val;
               handwritten_nums[i].value_vector[j] = cur_val;
               handwritten_nums[i].Number = NUMBER;
           }

         getline(in, line);

         if(i < handdata_size){
             ++i;
         } else{
           std::cout<<" INCREASE MEMORY FOR PARSED DATA! ";
         }

         }

       num_counts[NUMBER] = i;
       in.close();
       }

       ++NUMBER;
    }

}

void DrawArea::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawCoordSystem(&painter);
    drawInput(&painter);
    redraw_input_stretched_and_azimut(&painter);

}


void DrawArea::resizeEvent(QResizeEvent* /* event */) {
    int w = width();
    int h = height();
    if (
        w != 0 && h != 0 && (
            xSize == 0 || ySize == 0 ||
            xSize != width() || ySize != height()
        )
    ) {
        xSize = w;
        ySize = h;
        initializeMap();
    }
}

void DrawArea::initializeMap() {
    int w = width();
    if (w == 0)
        return;

    int h = height();
    if (h == 0)
        return;

    // We try to save the relative position of center
    double yCenter = (0. - ymin)/(ymax - ymin);

    double aspect = (double) h / (double) w;
    double dx = xmax - xmin;
    double dy = dx*aspect;
    ymin = 0. - dy*yCenter;
    ymax = ymin + dy;

    xCoeff = (double) w / dx;
    yCoeff = (double) h / dy;
}

QPoint DrawArea::map(const R2Point& p) const {
    return QPoint(
        floor((p.x - xmin)*xCoeff + 0.5),
        floor((ymax - p.y)*yCoeff + 0.5)
    );
}

QPointF DrawArea::mapF(const R2Point& p) const {
    return QPoint(
        (p.x - xmin)*xCoeff,
        (ymax - p.y)*yCoeff
    );
}

R2Point DrawArea::invMap(const QPointF& p) const {
    return R2Point(
        xmin + (p.x()/xCoeff),
        ymax - (p.y()/yCoeff)
    );
}

R2Point DrawArea::invMap(const QPoint& p) const {
    return R2Point(
        xmin + (p.x()/xCoeff),
        ymax - (p.y()/yCoeff)
    );
}


void DrawArea::onDraw() {
    if (n > 0) {
        update();   // Redraw the picture in window
    }
}

void DrawArea::drawCoordSystem(QPainter* qp) {
    int w = width();
    int h = height();
    if (w == 0 || h == 0)
        return;

    QColor lightGray(220, 220, 220);
    QColor gray(128, 128, 128);
    QColor black(0, 0, 0);

    qp->setBrush(QBrush(lightGray));
    qp->drawRect(0, 0, w, h);

    qp->setPen(gray);

    QPoint p0, p1;
    int ix = (int) xmin;
    while (ix <= (int) xmax) {
        if (ix == 0) {
            ++ix;
            continue;
        }
        p0 = map(R2Point(ix, ymin));
        p1 = map(R2Point(ix, ymax));
        qp->drawLine(p0, p1);
        ++ix;
    }

    int iy = (int) ymin;
    while (iy <= (int) ymax) {
        if (iy == 0) {
            ++iy;
            continue;
        }
        p0 = map(R2Point(xmin, iy));
        p1 = map(R2Point(xmax, iy));
        qp->drawLine(p0, p1);
        ++iy;
    }

    qp->setPen(black);
    p0 = map(R2Point(xmin, 0.));
    p1 = map(R2Point(xmin+(xmax-xmin)/10, 0.));
    qp->drawLine(p0, p1);

    qp->setPen(black);
    p0 = map(R2Point(xmax-(xmax-xmin)/10, 0.));
    p1 = map(R2Point(xmax, 0.));
    qp->drawLine(p0, p1);

    qp->setPen(black);
    p0 = map(R2Point(0., ymin));
    p1 = map(R2Point(0., ymin+(ymax-ymin)/10));
    qp->drawLine(p0, p1);

    qp->setPen(black);
    p0 = map(R2Point(0.,ymax-(ymax-ymin)/10));
    p1 = map(R2Point(0.,ymax));
    qp->drawLine(p0, p1);


}

void DrawArea::mousePressEvent(QMouseEvent* event) {
    input_is_on = true;
    R2Point p = invMap(event->pos());
    numPoints = 0;
    q[numPoints] = p;
    ++numPoints;

    if (numPoints <= 0 || catchState != CATCH_SHOW)
        return;

    catchState = CATCH_DRAG;
    catchInitialPos = invMap(event->pos());
    setCursor(Qt::OpenHandCursor);
    mainWindow->on_custom_spinBox_editingFinished();

    onDraw();

}


void DrawArea::redraw_input_stretched_and_azimut(QPainter* painter){

    QPen redPen(Qt::red);
    redPen.setWidth(3);

    QPen greenPen(Qt::green);
    greenPen.setWidth(3);

    double q_x_max(xmin), q_y_max(ymin);
    double x_left = xmin + (xmax-xmin)*0.05,
           x_right = xmax - (xmax-xmin)*0.05,
           y_up = ymax - (ymax-ymin)*0.05,
           y_down = ymin + (ymax-ymin)*0.05;

    for (int i = 0; i < numPoints; ++i){

        if (q[i].x > q_x_max)
            q_x_max = q[i].x;
        if (q[i].y > q_y_max)
            q_y_max = q[i].y;
    }

    for (int i = 0; i < numPoints; ++i){

        if (q[i].x > q_x_max)
            q_x_max = q[i].x;
        if (q[i].y > q_y_max)
            q_y_max = q[i].y;
    }

    double q_x_min( xmax ), q_y_min( ymax );

    for (int i = 0; i < numPoints; ++i){

        if (q[i].x < q_x_min)
            q_x_min = q[i].x;
        if (q[i].y < q_y_min)
            q_y_min = q[i].y;
    }

    for (int i = 0; i < numPoints; ++i){

        q_stretched[i].x = x_left + (q[i].x-q_x_min)*(x_right-x_left)/(q_x_max-q_x_min);
        q_stretched[i].y = y_down + (q[i].y-q_y_min)*(y_up - y_down)/(q_y_max-q_y_min);

    }

  //  fprintf(stderr, " %d (%d)   ", numPoints);

    for (int i = 1; i < numPoints; ++i) {
        painter->setPen(redPen);
        painter->drawLine(map(q_stretched[i]), map(q_stretched[i-1]));
    }

    for (int i = 0; i < numPoints-1; ++i) {
      q_azimut[i].x = q_stretched[i+1].x - q_stretched[i].x;
      q_azimut[i].y = q_stretched[i+1].y - q_stretched[i].y;
    }

    for (int i = 0; i < numPoints-1; ++i) {
        azimut[i] = acos(q_azimut[i].x / sqrt(q_azimut[i].y*q_azimut[i].y+q_azimut[i].x*q_azimut[i].x)) * (180. / PI);
    }


}

void DrawArea::mouseReleaseEvent(QMouseEvent* event) {
    input_is_on = false;
    R2Point p = invMap(event->pos());

    // User ended his inputting, we have azimut[] - array of numP-1 elements - here we interpolate it
      for(int i = 0; i < answer_size; ++i){
        answer[i] = azimut[ (int)(((double)(i) / (double)(answer_size))*
                ((double)(numPoints-1.))) ];
      }

    // Check "user-custom" button

      if (CUSTOM_INPUT == true){

          // Read inputed number from UI  when mouse_press
          // Save vector to txt-file:

          std::string num_str = std::to_string(drawArea->CUSTOM_NUMBER);
          std::string file_path = "./data/HANDWRITTEN_DATA_" +
                  num_str + ".txt";
          std::ofstream out(file_path, ios::app);

          if (out.is_open()){
            for (int i = 0; i < answer_size; ++i){
              out<<answer[i]<<" ";
            }
            out<<" Number = "<<drawArea->CUSTOM_NUMBER<<std::endl<<std::endl;

          }

      } else {
          // Make detection
       for (int k = 0; k < handdata_size; ++k){
            if(handwritten_nums[k].Number != -1){
                errors[k] = 0;
                int i = 0;
                while (i < 128 ){
                    ++i;
                    errors[k] += abs(handwritten_nums[k].value_vector[i] -
                                     answer[i]);
                }
            } else {
                errors[k] = -1;
            }

          }

          int min_err_idx = 0;
          int min_error = errors[0];

          for (int k = 0; k < handdata_size; ++k){
              if((min_error > errors[k]) && (errors[k] != -1)){
                  min_error = errors[k];
                  min_err_idx = k;
              }
          }

          mainWindow->set_answer(handwritten_nums[min_err_idx].Number);
      }

    setCursor(Qt::ArrowCursor);
    clear();
    update();
}



void DrawArea::mouseMoveEvent(QMouseEvent* event) {
    R2Point p = invMap(event->pos());
    /*
    printf(
        "Mouse move: pos=(%d, %d)\n",
        event->pos().x(), event->pos().y()
    );
    */
    int pointIdx = (-1);
    if (catchState != CATCH_DRAG) {
        pointIdx = findPoint(p);
    }

    if (catchState == CATCH_NONE) {
        if (pointIdx >= 0) {
            catchState = CATCH_SHOW;
            catchPointIdx = pointIdx;
            setCursor(Qt::OpenHandCursor);
        }
    } else if (catchState == CATCH_SHOW) {
        if (pointIdx < 0) {
            catchState = CATCH_NONE;
            catchPointIdx = (-1);
            setCursor(Qt::ArrowCursor);
        }
    } else if (catchState == CATCH_DRAG) {
        assert(catchPointIdx >= 0);
        if (catchPointIdx >= 0) {
            q[catchPointIdx] = p;
            onDraw();
        }
    }

    if ( (input_is_on == true ) &&
           ((( invMap(event->pos()).x - q[numPoints-1].x )*( invMap(event->pos()).x - q[numPoints-1].x ) +
           ( invMap(event->pos()).y - q[numPoints-1].y )*( invMap(event->pos()).y - q[numPoints-1].y ))) > 0.1 ){

        q[numPoints].x = invMap(event->pos()).x;
        q[numPoints].y = invMap(event->pos()).y;
        ++numPoints;
        update();

        if (numPoints == 10 ) {
            for (int i = 0; i < 10; ++i ){
              //  fprintf(stderr, " %f \n", q[i].x);
            }
        }

    }
}


void DrawArea::drawInput(QPainter* painter) {
    QPen bluePen(Qt::blue);
    bluePen.setWidth(3);

    QPen redPen(Qt::red);
    redPen.setWidth(3);

    QPen magentaPen(Qt::magenta);
    magentaPen.setWidth(2);

    for (int i = 1; i < numPoints; ++i) {
        painter->setPen(bluePen);
        painter->drawLine(map(q[i]), map(q[i-1]));
    }
}

int DrawArea::findPoint(const R2Point& pos) const {
    if (numPoints <= 0)
        return (-1);

    // Define the catch distance in math. coordinates
    QPoint q0(0, 0), q1(1, 0);
    double pixelSize = invMap(q0).distance(invMap(q1));
    double nearPointDist = 1e+30;   // Infinity
    int nearPointIdx = (-1);
    for (int i = 0; i < numPoints; ++i) {
        double d = pos.distance(q[i]);
        if (d < nearPointDist) {
            nearPointDist = d;
            nearPointIdx = i;
        }
    }
    if (nearPointDist > pixelSize*CATCH_DISTANCE) {
        return (-1);
    } else {
        return nearPointIdx;
    }
}

void DrawArea::clear() {
    numPoints = 0;
    n = 0;
    update();
}

