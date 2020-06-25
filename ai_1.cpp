#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Point {
    int x, y, h;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) ,h(0) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board; // 此時的棋盤樣貌
std::vector<Point> next_valid_spots;

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

int verify_type(Point p, bool colored)
{
    int k = colored ?-1:1;
    if( p == Point(0,0) || p == Point(0,7) || p == Point(7,0) || p == Point(7,7) )
        return 2*k;
    else if( (p.x == 0 || p.x == 7)&&( p.y >= 2 && p.y <= 5 ) )
        return 1*k;
    else if( p == Point(0,1) || p == Point(1,0) || p == Point(0,6) || p == Point(1,7)
            || p == Point(6,0) || p == Point(7,1) || p == Point(7,0) || p == Point(6,7) )
        return -2*k;
    else if( p == Point(1,1) || p == Point(1,6) || p == Point(6,1) || p == Point(6,6) )
        return -1*k;
    else return 0;
}

/*
// black: minimizing, colored
// white: maximizing, un-colored
int minmax(Point pos, int depth, bool player){
    if(depth == 0){

    }
}*/

void write_valid_spot(std::ofstream& fout) {
    //int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    
    // ===================================
    // find good moves here
    // i'm black, colored, minimizer
    int nowmin = INT16_MAX;
    Point p;
    for(auto i:next_valid_spots){
        if( verify_type(i,true) < nowmin ){
            nowmin = verify_type(i,true);
            p = i;
        }
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
