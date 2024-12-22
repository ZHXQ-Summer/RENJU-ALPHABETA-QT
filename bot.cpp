#include<iostream>
#include<climits>
#include<algorithm>
#include<fstream>
#include<cstring>
#include"jsoncpp/json.h"
using namespace std;
struct node{
    int x,y,score;
};
const int margin=20;//边
const int cel=40;//方格大小
const int chessr = 18;//棋子半径
const int five=999990000,live_four=10000000,dead_four=1000000,live_three=1000000,dead_three=1000,live_two=100,dead_two=50,one=1;
int dp[8][2]={0,1,0,-1,1,0,-1,0,-1,-1,1,1,1,-1,-1,1};//八个方向
    int flag;
    int chessboard[16][16]={};
    node step[400]={};
    int step_num=0;
    int cur_player=1;
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
    void placeAt(int x, int y)
    {
        if (x >= 0 && y >= 0) {
            chessboard[x][y] = cur_player;
            cur_player=-cur_player;
        }
    }
    int main(){
        // 读入JSON
        string str;
        getline(cin, str);
        Json::Reader reader;
        Json::Value input;
        reader.parse(str, input);
        // 分析自己收到的输入和自己过往的输出，并恢复状态
        int turnID = input["responses"].size();
        for (int i = 0; i < turnID; i++) {
            placeAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt());
            placeAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt());
        }
        placeAt(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt());
        // 输出决策JSON
        Json::Value ret;

            Json::Value action;
            node move=a_b(-INT_MAX,INT_MAX,3,true);
            action["x"] = move.x;
            action["y"] = move.y;
            ret["response"] = action;

        Json::FastWriter writer;
        cout << writer.write(ret) << endl;
        return 0;
    }
bool winner_check(int x,int y,int cur_player){//判断赢家
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
node a_b(int alpha,int beta,bool maximize,int depth){//阿尔法贝塔剪枝算法，阿尔法是最大下界，贝塔是最小上界，布尔型变量决定取小取大，depth控制递归深度
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
bool Node_check(node a,int cur_player){//ai检验该位置是否应该下棋
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

int evaluate(){//计算局面分数
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
int extract(int a[],int len){//读取棋链
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
int evaluate_line(int a[],int start_index,int end_index){//棋链计分
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
int judge(int a[],int max_co,int start_index,int end_index,int start,int end){//棋形判断
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
int evaluate_line2(int a[],int start_index,int end_index){//对手棋链计分
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
int judge2(int a[],int max_co,int start_index,int end_index,int start,int end){//对手棋形判断
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
bool longchain_check(int x,int y,int cur_player){//判断长连禁手
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
bool threefour_check(int x,int y,int cur_player){//判断三三四四禁手
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
int judge_three_four(int x,int a[],int len,int cur_player){//检查三三四四
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
