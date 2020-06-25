#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "function.hpp"

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board; // 此時的棋盤樣貌
std::vector<Point> next_valid_spots;
std::array<Point, 4> corners{{
        Point(0, 0), Point(0,7), Point(7,0),Point(7,7)
    }
};


void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    float x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

int heuristic(OthelloBoard cur, bool colored){
    int heuristic = 0;
    int bk = 0, wh = 0;
    for(auto i:corners){
        if( cur.board[i.x][i.y] == 1)
            bk ++;
        else if(cur.board[i.x][i.y] == 2) 
            wh ++;
    }
    heuristic += 100*(wh-bk)/ (bk+wh);

    // 連線
    bool prev_color = true;
    int consecutive_bk = 0;
    int consecutive_wh = 0;
    // 可在棋盤邊row上連線 左到右
    for(int i = 0; i < SIZE; i = i+7){
        for(int j = 0; j < SIZE; j ++){
            if(!cur.board[i][0])
                break;
            if(consecutive_bk == 0 && consecutive_wh == 0){
                prev_color = cur.board[i][j]==1 ; 
                if(prev_color) consecutive_bk++;
                else consecutive_wh++;
            }
            else{
                if(prev_color && cur.board[i][j]==1)
                    consecutive_bk ++;
                else if( !prev_color && cur.board[i][j]==2)
                    consecutive_wh ++;
                else break;
            }
        }
    }
    if(consecutive_bk >= 2) heuristic += 50*consecutive_bk*(-1);
    if(consecutive_wh >= 2) heuristic += 50*consecutive_wh;

    // 右到左
    consecutive_bk = 0;
    consecutive_wh = 0;
    for(int i = 0; i < SIZE; i = i+7){
        for(int j = 7; j >= 0; j --){
            if(!cur.board[i][j])
                break;
            if(consecutive_bk == 0 && consecutive_wh == 0){
                prev_color = cur.board[i][j]==1 ; 
                if(prev_color) consecutive_bk++;
                else consecutive_wh++;
            }
            else{
                if(prev_color && cur.board[i][j]==1)
                    consecutive_bk ++;
                else if( !prev_color && cur.board[i][j]==2)
                    consecutive_wh ++;
                else break;
            }
        }
    }
    if(consecutive_bk >= 2) heuristic += 50*consecutive_bk*(-1);
    if(consecutive_wh >= 2) heuristic += 50*consecutive_wh;

    consecutive_bk = 0;
    consecutive_wh = 0;
    // 可在棋盤邊col上連線 上到下
    for(int j = 0; j < SIZE; j = j+7){
        for(int i = 0; i < SIZE; i ++){
            if(!cur.board[0][j])
                break;
            if(consecutive_bk == 0 && consecutive_wh == 0){
                prev_color = cur.board[i][j]==1 ; 
                if(prev_color) consecutive_bk++;
                else consecutive_wh++;
            }
            else{
                if(prev_color && cur.board[i][j]==1)
                    consecutive_bk ++;
                else if( !prev_color && cur.board[i][j]==2)
                    consecutive_wh ++;
                else break;
            }
        }
    }
    if(consecutive_bk >= 2) heuristic += 50*consecutive_bk*(-1);
    if(consecutive_wh >= 2) heuristic += 50*consecutive_wh;

    consecutive_bk = 0;
    consecutive_wh = 0;
    // 可在棋盤邊col上連線 下到上
    for(int j = 0; j < SIZE; j = j+7){
        for(int i = 7; i >= 0; i --){
            if(!cur.board[0][j])
                break;
            if(consecutive_bk == 0 && consecutive_wh == 0){
                prev_color = cur.board[i][j]==1 ; 
                if(prev_color) consecutive_bk++;
                else consecutive_wh++;
            }
            else{
                if(prev_color && cur.board[i][j]==1)
                    consecutive_bk ++;
                else if( !prev_color && cur.board[i][j]==2)
                    consecutive_wh ++;
                else break;
            }
        }
    }
    if(consecutive_bk >= 2) heuristic += 50*consecutive_bk*(-1);
    if(consecutive_wh >= 2) heuristic += 50*consecutive_wh;

    return heuristic;
}


// black: minimizer, colored
// white: maximizer, un-colored
int minmax(OthelloBoard cur, int depth, bool colored){
    if(depth == 0){

    }
}

void write_valid_spot(std::ofstream& fout) {
    //int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    
    // ===================================
    // find good moves here
    // note that black=1 =colored =minimizer
    Point p;

    // starting to build root 
    // first build root state
    OthelloBoard cur(board);
    cur.cur_player = player;
    Node* root;
    root->state = cur;
    
    // ===================================

    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
