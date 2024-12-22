#include "widget.h"
#include "ui_widget.h"
#include <QWidget>
#include<QMessageBox>
#include<QGraphicsWidget>
#include<QGraphicsScene>
#include<QGraphicsView>
#include<Qpushbutton>
#include<Qdebug>
#include<Qpen>
#include<Qbrush>
#include<QMouseEvent>
#include<QPointF>
#include<iostream>
#include<climits>
#include<algorithm>
#include<fstream>
using namespace std;
const int margin=20;//边
const int cel=40;//方格大小
const int chessr = 18;//棋子半径
const int five=999990000,live_four=10000000,dead_four=1000000,live_three=1000000,dead_three=1000,live_two=100,dead_two=50,one=1;
int dp[8][2]={0,1,0,-1,1,0,-1,0,-1,-1,1,1,1,-1,-1,1};//八个方向
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setMinimumSize(1000,700);
    setMaximumSize(1000,700);
    ui->start_button->setText(tr("游戏开始"));
    connect(ui->start_button,SIGNAL(clicked()),this,SLOT(choose()));//开始按钮与黑白棋选择
    connect(ui->restart_button,SIGNAL(clicked()),this,SLOT(restart()));
    connect(ui->withdraw_button,SIGNAL(clicked()),this,SLOT(withdraw()));
    connect(ui->save_Button,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->load_button,SIGNAL(clicked()),this,SLOT(load()));
    connect(ui->close_button,SIGNAL(clicked()),this,SLOT(close()));
    scene=new QGraphicsScene(this);
    for(int i=0;i<14;i++){
        for(int j=0;j<14;j++){
            QGraphicsRectItem* rec=new QGraphicsRectItem(margin+i*cel,margin+j*cel,cel,cel);//画出矩形
            rec->setPen(QPen(Qt::black,1));//边
            scene->addItem(rec);//在graphicscene中绘制多个矩形作为棋盘
        }
    }
    ui->graphicview->setScene(scene);//将scene加入graphicview中，在屏幕上显示

}

Widget::~Widget()
{
    delete ui;
}

void Widget::choose(){//通过弹窗让用户选择下黑棋或白棋
    if(start_check){
        return;
    }
    QMessageBox::StandardButton box;
    box=QMessageBox::question(this,tr("黑白棋选择"),tr("你要选择黑棋吗"),
                          QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
    if(box==QMessageBox::Yes){
        flag=1;
    }
    else{
        flag=-1;
    }
    start_check=true;
    if(flag==-1){
        node point=a_b(-INT_MAX,INT_MAX,true,7);
        chessboard[point.x][point.y]=cur_player;
        QGraphicsEllipseItem *chess= new QGraphicsEllipseItem(margin+point.x*cel-cel/2,margin+point.y*cel-cel/2,chessr*2,chessr*2);//棋子
        chess->setPen(QPen(Qt::black,1));
        chess->setBrush(QBrush(cur_player==1?Qt::black:Qt::white));//绘制棋子
        Widget::scene->addItem(chess);
        cur_player=-cur_player;
    }
}
void Widget::mousePressEvent(QMouseEvent *event){//落子
    if(!start_check){
        QMessageBox::warning(this,tr("游戏未开始"),tr("请点击游戏开始"));
        return;
    }
    if(flag==cur_player){
    QPointF coord=ui->graphicview->mapToScene(event->pos());
    int x=(coord.x())/40;
    int y=(coord.y())/40;
    if(x<0||x>14||y<0||y>14){//是否合法
        return;
    }
    if((longchain_check(x,y,cur_player)||threefour_check(x,y,cur_player))&&!(winner_check(x,y,cur_player)&&!longchain_check(x,y,cur_player))){
        QMessageBox::warning(this,tr("犯规"),tr("此处禁手"));
        return;
    }
    if(chessboard[x][y]==0){
        step[step_num].x=x,step[step_num].y=y;
        step_num++;
        chessboard[x][y]=cur_player;
        QGraphicsEllipseItem *chess= new QGraphicsEllipseItem(margin+x*cel-cel/2,margin+y*cel-cel/2,chessr*2,chessr*2);//棋子
        chess->setPen(QPen(Qt::black,1));
        chess->setBrush(QBrush(cur_player==1?Qt::black:Qt::white));//绘制棋子
        Widget::scene->addItem(chess);
        if(winner_check(x,y,cur_player)&&!longchain_check(x,y,cur_player)){
            QMessageBox::StandardButton buttonover=QMessageBox::information(this,tr("游戏结束"),cur_player==1?tr("黑棋胜利"):tr("白棋胜利"));
            if(buttonover==QMessageBox::Ok){
                restart();
            }
            return;
        }
        if(tie()){
            QMessageBox::StandardButton buttontie=QMessageBox::information(this,tr("游戏结束"),tr("平局！"));
            if(buttontie==QMessageBox::Ok){
                restart();
            }
            return;
        }
        cur_player=-cur_player;//换手
        flag=-flag;
        node point=a_b(-INT_MAX,INT_MAX,true,3);
        chessboard[point.x][point.y]=cur_player;
        step[step_num].x=point.x,step[step_num].y=point.y;
        step_num++;
        QGraphicsEllipseItem *chess_ai= new QGraphicsEllipseItem(margin+point.x*cel-cel/2,margin+point.y*cel-cel/2,chessr*2,chessr*2);//棋子
        chess_ai->setPen(QPen(Qt::black,1));
        chess_ai->setBrush(QBrush(cur_player==1?Qt::black:Qt::white));//绘制棋子
        Widget::scene->addItem(chess_ai);
        if(winner_check(point.x,point.y,cur_player)){
            QMessageBox::StandardButton buttonover=QMessageBox::information(this,tr("游戏结束"),cur_player==1?tr("黑棋胜利"):tr("白棋胜利"));
            if(buttonover==QMessageBox::Ok){
                restart();
            }
            return;
        }
        if(tie()){
            QMessageBox::StandardButton buttontie=QMessageBox::information(this,tr("游戏结束"),tr("平局！"));
            if(buttontie==QMessageBox::Ok){
                restart();
            }
            return;
        }
        cur_player=-cur_player;
        flag=-flag;
    }
    }
    else{
        return;
    }
}
bool Widget::winner_check(int x,int y,int cur_player){//判断赢家
    int i, j, count;

    // 判断横向是否五子连珠
    count = 1;
    for (i = x - 1; i >= 0 && chessboard[i][y] == cur_player; i--) {
        count++;
    }
    for (i = x + 1; i <15 && chessboard[i][y] == cur_player; i++) {
        count++;
    }
    if (count >= 5) {
        return true;
    }
    // 判断纵向是否五子连珠
    count = 1;
    for (j = y - 1; j >= 0 && chessboard[x][j] == cur_player; j--) {
        count++;
    }
    for (j = y + 1; j < 15 && chessboard[x][j] == cur_player; j++) {
        count++;
    }
    if (count >= 5) {
        return true;
    }

    // 判断左上到右下是否五子连珠
    count = 1;
    for (i = x - 1, j = y - 1; i >= 0 && j >= 0 && chessboard[i][j] == cur_player; i--, j--) {
        count++;
    }
    for (i = x + 1, j = y + 1; i < 15 && j < 15 && chessboard[i][j] == cur_player; i++, j++) {
        count++;
    }
    if (count >= 5) {
        return true;
    }

    // 判断左下到右上是否五子连珠
    count = 1;
    for (i = x - 1, j = y + 1; i >= 0 && j < 15 && chessboard[i][j] == cur_player; i--, j++) {
        count++;
    }
    for (i = x + 1, j = y - 1; i < 15 && j >= 0 && chessboard[i][j] == cur_player; i++, j--) {
        count++;
    }
    if (count >= 5) {
        return true;
    }

    return false;
}
bool Widget::tie(){//判断平局
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            if(chessboard[i][j]==0){
                return false;
            }
        }
    }
    return true;
}
void Widget::restart(){//重新开始游戏
    scene->clear();
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            chessboard[i][j]=0;//清空棋盘
        }
    }
    for(int i=0;i<14;i++){//重新绘制格子
        for(int j=0;j<14;j++){
            QGraphicsRectItem* rec=new QGraphicsRectItem(margin+i*cel,margin+j*cel,cel,cel);//画出矩形
            rec->setPen(QPen(Qt::black,1));//边
            scene->addItem(rec);//在graphicscene中绘制多个矩形作为棋盘
        }
    }
    for(int i=0;i<step_num;i++){
        step[i].x=0,step[i].y=0;
    }
    step_num=0;
    cur_player=1;//黑方先手
    start_check=false;
}
node Widget::a_b(int alpha,int beta,bool maximize,int depth){//阿尔法贝塔剪枝算法，阿尔法是最大下界，贝塔是最小下界，布尔型变量决定取小取大，depth控制递归深度
    node point,bestpoint;
    if(chessboard[7][7]==0){
        point.x=7;
        point.y=7;
        return point;
    }
    if(depth==0){
                    point.x=0;
                    point.y=0;
                    point.score=evaluate();
                    return point;
                }
        if(maximize){
            bestpoint.score=-INT_MAX;
            for(int i=0;i<15;i++){
                for(int j=0;j<15;j++){
                    point.x=i;
                    point.y=j;
                    if(Node_check(point,cur_player)){
                        if(winner_check(i,j,cur_player)){
                            point.score=2000000000;
                            return point;
                        }
                        chessboard[i][j]=cur_player;
                        node newpoint=a_b(alpha,beta,false,depth-1);
                        newpoint.x=i;
                        newpoint.y=j;
                        chessboard[i][j]=0;
                        if(newpoint.score>bestpoint.score){
                            bestpoint=newpoint;
                        }
                        alpha=max(alpha,bestpoint.score);
                        if(beta<=alpha){
                            goto loop;//剪枝
                        }
                    }
                }
            }
        }
        else{
            bestpoint.score=INT_MAX;
            for(int i=0;i<15;i++){
                for(int j=0;j<15;j++){
                    point.x=i;
                    point.y=j;
                    if(Node_check(point,-cur_player)){
                        if(winner_check(i,j,-cur_player)){
                            point.score=-2000000000;
                            return point;
                        }
                        chessboard[i][j]=-cur_player;
                        node newpoint=a_b(alpha,beta,true,depth-1);
                        chessboard[i][j]=0;
                        newpoint.x=i;
                        newpoint.y=j;
                        if(newpoint.score<bestpoint.score){
                            bestpoint=newpoint;
                        }
                        beta=min(beta,bestpoint.score);
                        if(beta<=alpha){
                            goto loop;
                        }
                    }
                }
            }
        }
loop:
    return bestpoint;
}
bool Widget::Node_check(node a,int cur_player){//ai检验该位置是否应该下棋
    int t=0;
    if(chessboard[a.x][a.y]!=0){//是否空
        return false;
    }
    for(int i=0;i<8;i++){//旁边是否空
        if(a.x+dp[i][0]>=0&&a.x+dp[i][0]<15&&a.y+dp[i][1]>=0&&a.y+dp[i][1]<15&&chessboard[a.x+dp[i][0]][a.y+dp[i][1]]!=0){
            t++;
        }
    }
    if(t==0){
        return false;
    }//是否犯规
    if((longchain_check(a.x,a.y,cur_player)||threefour_check(a.x,a.y,cur_player))&&!(winner_check(a.x,a.y,cur_player)&&!longchain_check(a.x,a.y,cur_player))){
        return false ;
    }
    return true;
    }

int Widget::evaluate(){//计算局面分数
    int score=0;
    for(int i=0;i<15;i++){//水平
        int a[20];
        for(int j=0;j<15;j++){
            a[j]=chessboard[i][j];

        }
        score+=extract(a,15);
    }
    for(int i=0;i<15;i++){//垂直
        int a[20];
        for(int j=0;j<15;j++){
            a[j]=chessboard[j][i];
        }
        score+=extract(a,15);
    }
    for(int i=0;i<15;i++){//左上右下
        int a[20],j=i,k=0;
        for(;j<15;j++,k++){
            a[k]=chessboard[k][j];
        }
        score+=extract(a,k);
    }
    for(int i=1;i<15;i++){
        int a[20],j=0,k=i;
        for(;k<15;j++,k++){
            a[j]=chessboard[k][j];
        }
        score+=extract(a,j);
    }
    for (int k = 0; k < 15; ++k) {//左下右上
        int i = k;
        int j = 14;
        int a[20];
        while (i < 15 && j >= 0) {
            a[i-k] = chessboard[i][j];
            ++i;
            --j;
        }
        score+=extract(a,15-k);
    }
    for (int k = 1; k < 15; ++k) {
        int i = 0;
        int j = 14 - k;
        int a[20];
        while (i < 15 && j >= 0) {
            a[i] = chessboard[i][j];
            ++i;
            --j;
        }
        score+=extract(a,15-k);
    }
    return score;
}
int Widget::extract(int a[],int len){//读取棋链
    if(len<5){
        return 0;
    }
    int start_index=-1,end_index=len,score=0;
    for(int i=0;i<len;i++){
        if(a[i]==-cur_player){
            score+=evaluate_line(a,start_index,i);
            start_index=i;
        }
    }
    score+=evaluate_line(a,start_index,end_index);
    start_index=-1,end_index=len;
    for(int i=0;i<len;i++){
        if(a[i]==cur_player){
            score-=evaluate_line2(a,start_index,i);
            start_index=i;
        }
    }
    score-=evaluate_line2(a,start_index,end_index);
    return score;
}
int Widget::evaluate_line(int a[],int start_index,int end_index){//棋链计分
    if(end_index-start_index-1<5){
        return 0;
    }
    int mid_empty=0,now_co=0,all_co=0,max_co=0,empty_check=0,score=0,start,end;
    for(int j=start_index+1;j<end_index;j++){
        if(a[j]==0){
            if(all_co==0){
                continue;
            }
            else{
                score+=judge(a,all_co,start_index,end_index,start,j);
                all_co=0;
            }
        }
        else{
            if(all_co==0){
                start=j;
            }
            all_co++;
        }
        if(j==end_index-1&&all_co!=0){
            score+=judge(a,all_co,start_index,end_index,start,j);
        }
    }
    return score;
}
int Widget::judge(int a[],int max_co,int start_index,int end_index,int start,int end){//棋形判断
    int check=(start!=start_index+1)+(end!=end_index-1);
    if(max_co>=5){
        return five;
    }

    if(max_co==4&&check==2){
        return dead_four;
    }
    if(max_co==4&&check==1){
        return 0;
    }

    if(max_co==3&&check==2){
        return dead_three;
    }
    if(max_co==3&&check==1){
        return dead_three;
    }

    if(max_co==2&&check==2){
        return live_two;
    }
    if(max_co==2&&check==1){
        return dead_two;
    }
    if(max_co==1&&check==2){
        return one;
    }
    return 0;
}
int Widget::evaluate_line2(int a[],int start_index,int end_index){//对手棋链计分
    if(end_index-start_index-1<5){
        return 0;
    }
    int mid_empty=0,now_co=0,all_co=0,max_co=0,empty_check=0,score=0,start,end;
    for(int j=start_index+1;j<end_index;j++){
        if(a[j]==0){
            if(all_co==0){
                continue;
            }
            else{
                score+=judge2(a,all_co,start_index,end_index,start,j);
                all_co=0;
            }
        }
        else{
            if(all_co==0){
                start=j;
            }
            all_co++;
        }
        if(j==end_index-1&&all_co!=0){
            score+=judge2(a,all_co,start_index,end_index,start,j);
        }
    }
    return score;
}
int Widget::judge2(int a[],int max_co,int start_index,int end_index,int start,int end){//对手棋形判断
    int check=(start!=start_index+1)+(end!=end_index-1);
    if(max_co>=5){
        return five;
    }
    if(max_co==4&&check==2){
        return five;
    }
    if(max_co==4&&check==1){
        return five;
    }
    if(max_co==3&&check==2){
        return five;
    }
    if(max_co==3&&check==1){
        return dead_three;
    }

    if(max_co==2&&check==2){
        return live_two;
    }
    if(max_co==2&&check==1){
        return dead_two;
    }
    if(max_co==1&&check==2){
        return one;
    }
    return 0;
}
bool Widget::longchain_check(int x,int y,int cur_player){//判断长连禁手
    int i, j, count;
    if(cur_player==-1){
        return false;
    }
    // 判断横向
    count = 1;
    for (i = x - 1; i >= 0 && chessboard[i][y] == 1; i--) {
        count++;
    }
    for (i = x + 1; i <15 && chessboard[i][y] == 1; i++) {
        count++;
    }
    if (count > 5) {
        return true;
    }
    // 判断纵向
    count = 1;
    for (j = y - 1; j >= 0 && chessboard[x][j] == 1; j--) {
        count++;
    }
    for (j = y + 1; j < 15 && chessboard[x][j] == 1; j++) {
        count++;
    }
    if (count > 5) {
        return true;
    }

    // 判断左上到右下
    count = 1;
    for (i = x - 1, j = y - 1; i >= 0 && j >= 0 && chessboard[i][j] == 1; i--, j--) {
        count++;
    }
    for (i = x + 1, j = y + 1; i < 15 && j < 15 && chessboard[i][j] == 1; i++, j++) {
        count++;
    }
    if (count > 5) {
        return true;
    }

    // 判断左下到右上
    count = 1;
    for (i = x - 1, j = y + 1; i >= 0 && j < 15 && chessboard[i][j] == cur_player; i--, j++) {
        count++;
    }
    for (i = x + 1, j = y - 1; i < 15 && j >= 0 && chessboard[i][j] == cur_player; i++, j--) {
        count++;
    }
    if (count > 5) {
        return true;
    }

    return false;
}
bool Widget::threefour_check(int x,int y,int cur_player){//判断三三四四禁手
    int i, j, count,fourfour=0,threethree=0;
    if(cur_player==-1){
        return false;
    }
    int a[20];
    for (i = 0; i <15; i++) {//水平
        a[i]=chessboard[i][y];
    }
    int check=judge_three_four(x,a,15,cur_player);
    if(check==dead_four){
        fourfour++;
    }
    if(check==live_three){
        threethree++;
    }
    for (i = 0; i <15; i++) {//垂直
        a[i]=chessboard[x][i];
    }
    check=judge_three_four(x,a,15,cur_player);
    if(check==dead_four){
        fourfour++;
    }
    if(check==live_three){
        threethree++;
    }
    memset(a,0,sizeof(a));
    if(y>=x)//左上右下
    {
        int j=y-x,k=0;
        for(;j<15;j++,k++){
            a[k]=chessboard[k][j];
        }
       check = judge_three_four(x,a,k,cur_player);

    }
   else{
        int a[20],j=0,k=x-y;
        for(;k<15;j++,k++){
            a[j]=chessboard[k][j];
        }
        check = judge_three_four(y,a,j,cur_player);
    }
    if(check==dead_four){
        fourfour++;
    }
    if(check==live_three){
        threethree++;
    }
    memset(a,0,sizeof(a));
    if(x+y<=14){
        int j,k;
        for(j=0,k=x+y;j<15&&k>=0;j++,k--){
            a[j]=chessboard[k][j];
        }
        check = judge_three_four(y,a,j,cur_player);
    }
    else{
        int j,k;
        for(k=14,j=x+y-14;k>=0&&j<15;k--,j++){
            a[j-x-y+14]=chessboard[k][j];
        }
        check=judge_three_four(14-x,a,14-k,cur_player);
    }
    if(check==dead_four){
        fourfour++;
    }
    if(check==live_three){
        threethree++;
    }
    if(threethree>=2||fourfour>=2){
        return true;
    }
    return false;
}
int Widget::judge_three_four(int x,int a[],int len,int cur_player){//检查三三四四
    int i,j,main=0,side_left=0,side_right=0;
    for(i=x;i>=0&&a[i]==cur_player;i--){
        main++;
    }
    for(i=i-1;i>=0&&a[i]==cur_player;i--){
        side_left++;
    }
    for(j=x+1;j<len&&a[j]==cur_player;j++){
        main++;
    }
    for(j=j+1;j<len&&a[j]==cur_player;j++){
        side_right++;
    }
    if(main+side_right+side_left>=4){
        return 0;
    }
    if(main+side_right==4||main+side_left==4){
        return dead_four;
    }
    if((main+side_right==3||main+side_left==3)&&j<len&&a[j]==0&&i>=0&&a[i]==0){
        return live_three;
    }
    return 0;
}
void Widget::save(){//存档
    ofstream fout("board.txt");
    if(!fout){
        cout<<1;
        return;
    }
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            fout<<chessboard[i][j]<<" ";
        }
    }
    fout<<cur_player<<" "<<flag;
    fout<<" "<<step_num;
    for(int i=0;i<step_num;i++){
        fout<<" "<<step[i].x<<" "<<step[i].y;
    }
    QMessageBox::information(this,tr("存档"),tr("存档成功"));
    fout.close();
    return;
}
void Widget::load(){//读档
    ifstream fin("board.txt");
    if(!fin){
        cout<<2;
        return;
    }
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            fin>>chessboard[i][j];
        }
    }
    fin>>cur_player>>flag;
    fin>>step_num;
    for(int i=0;i<step_num;i++){
        fin>>step[i].x>>step[i].y;
    }
    fin.close();
    scene->clear();
    for(int i=0;i<14;i++){//重新绘制格子
        for(int j=0;j<14;j++){
            QGraphicsRectItem* rec=new QGraphicsRectItem(margin+i*cel,margin+j*cel,cel,cel);//画出矩形
            rec->setPen(QPen(Qt::black,1));//边
            scene->addItem(rec);//在graphicscene中绘制多个矩形作为棋盘
        }
    }
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            if(chessboard[i][j]!=0){
            QGraphicsEllipseItem *chess= new QGraphicsEllipseItem(margin+i*cel-cel/2,margin+j*cel-cel/2,chessr*2,chessr*2);//棋子
            chess->setPen(QPen(Qt::black,1));
            chess->setBrush(QBrush(chessboard[i][j]==1?Qt::black:Qt::white));//绘制棋子
            Widget::scene->addItem(chess);}
        }
    }
    start_check=true;
    return;
}
void Widget::withdraw(){
    if(step_num<2){
        return;
    }
    int x=step[step_num-1].x,y=step[step_num-1].y;
    chessboard[x][y]=0;
    QGraphicsItem* ai_item = scene->itemAt(
            margin+x*cel,margin+y*cel,
            QTransform());
    if (ai_item != nullptr) {
        scene->removeItem(ai_item);
        delete ai_item;
    }
    step_num--;
    x=step[step_num-1].x,y=step[step_num-1].y;
    chessboard[x][y]=0;
    QGraphicsItem* item = scene->itemAt(
        margin+x*cel,margin+y*cel,
        QTransform());
    if (item != nullptr) {
        scene->removeItem(item);
        delete item;
    }
    step_num--;
}
