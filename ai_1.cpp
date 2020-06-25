#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "function.hpp"
using namespace std;

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board; // 此時的棋盤樣貌
std::vector<Point> next_valid_spots;
std::array<Point, 4> corners{{
        Point(0, 0), Point(0,7), Point(7,0),Point(7,7)
    }
};
Point ans(-1,-1);

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

int heuristic(OthelloBoard cur,bool maximizer){
    int heuristic = 0;
    int bk = 0, wh = 0;
    for(auto i:corners){
        if( cur.board[i.x][i.y] == 1)
            bk ++;
        else if(cur.board[i.x][i.y] == 2) 
            wh ++;
    }
    if(maximizer)
        heuristic += 100*(bk-wh)/ (bk+wh+1);
    else heuristic += -100*(wh-bk)/ (bk+wh+1);

    /*
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
    */

    return heuristic;
}


int minimax(OthelloBoard curnode, int depth, bool maximizer){
    if(depth == 0){
        // return static evaluation of position
        curnode.heuristic = heuristic(curnode,maximizer);
        return curnode.heuristic;
    }
    
    if(maximizer){
        int maxeval = INT16_MIN;
        for(auto i:curnode.next_valid_spots){
            OthelloBoard next = curnode;
            if(!next.put_disc(i)){
                cout << "minimax-maximizer put_disc error\n";
                return -1;
            }
            else{
                int eval = minimax( next,depth-1,false); 
                maxeval = max(maxeval,eval);
                if(maxeval == eval)
                    ans = i;
            }
        }
        curnode.heuristic = maxeval;
        return maxeval;
    }
    else{
        int mineval = INT16_MAX;
        for(auto i:curnode.next_valid_spots){
            OthelloBoard next = curnode;
            if(!next.put_disc(i)){
                cout << "minimax-minimizer put_disc error\n";
                return -1;
            }
            else{
                int eval = minimax( next,depth-1,true);
                mineval = min(mineval,eval);
                if(mineval == eval)
                    ans = i;
            }
        }
        curnode.heuristic = mineval;
        return mineval;
    }

} // end function

void write_valid_spot(std::ofstream& fout) {
    //int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    Point p;
    // ===================================
    // find good moves here
    // black =1 =colored =maximizer
    // starting to build root 
    OthelloBoard cur(board);
    cur.cur_player = player;
    cur.next_valid_spots = next_valid_spots;
    cur.heuristic = minimax(cur,20,player==1);
    
    if(ans.x == -1 && ans.y == -1)
        cout << "non-use tree search\n";
    else {
        p.x = ans.x;
        p.y = ans.y;
    }
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
