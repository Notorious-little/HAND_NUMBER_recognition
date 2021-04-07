#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <locale.h>
#include <iostream>

// Global variables
MainWindow* mainWindow = 0;
DrawArea* drawArea = 0;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow){

    setlocale(LC_ALL, "C");
    mainWindow = this;
    ui->setupUi(this);

}


MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::on_clearButton_clicked(){
    drawArea->clear();
}

void MainWindow::on_drawButton_clicked(){
  //  SET CUSTOM N
    drawArea->onDraw();
}

void MainWindow::on_closeButton_clicked(){
    QApplication::quit();
}

void MainWindow::on_parseButton_clicked(){
    drawArea->parce_handwr_data_from_txt();
}

void MainWindow::on_custom_checkbox_clicked(){
    drawArea->CUSTOM_INPUT =
        ui->custom_checkbox->isChecked();
    on_custom_spinBox_editingFinished();

}

void MainWindow::on_custom_spinBox_editingFinished(){
    drawArea->CUSTOM_NUMBER =
        ui->custom_spinBox->value();
}

void MainWindow::set_answer(int ans){
    QString str_ans = QString::number(ans);
    ui->answer_field->setText(str_ans);
}
