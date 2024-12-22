#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QMessageBox>
#include<QGraphicsWidget>
#include<QGraphicsScene>
#include<QGraphicsView>
#include<Qpushbutton>
#include<Qdebug>
#include<iostream>
#include<Qpen>
#include<Qbrush>
#include<QMouseEvent>
#include<QPointF>
#include<climits>
#include<algorithm>
#include<fstream>
using namespace std;
struct node{
    int x,y,score;
};

class QMessageBox;
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void choose();
    void save();
    void load();
    void restart();
    void withdraw();


private:
    Ui::Widget *ui;
    int flag;
    int chessboard[16][16]={};
    node step[400]={};
    int step_num=0;
    QGraphicsView graphicview;
    void mousePressEvent(QMouseEvent *event);
    int cur_player=1;
    QGraphicsScene *scene;
    bool winner_check(int x,int y,int cur_player);
    bool tie();

    bool start_check=false;
    node a_b(int alpha,int beta,bool maximize,int depth);
    bool Node_check(node a,int cur_player);
    int evaluate();
    int evaluate_line(int a[],int start_index,int end_index);
    int extract(int a[],int len);
    int judge(int a[],int max_co,int start_index,int end_index,int start,int end);
    int evaluate_line2(int a[],int start_index,int end_index);
    int judge2(int a[],int max_co,int start_index,int end_index,int start,int end);
    bool longchain_check(int x,int y,int cur_player);
    bool threefour_check(int x,int y,int cur_player);
    int judge_three_four(int x,int a[],int len,int cur_player);
};
#endif // WIDGET_H
