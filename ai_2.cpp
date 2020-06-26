#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include "function.hpp"
using namespace std;

int player;
const int SIZE = 8;
const int MaxDepth = 7;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
std::array<Point, 4> corners{{
        Point(0, 0), Point(0,7), Point(7,0),Point(7,7)
    }
};
std::array<Point, 8> c_spots{{
        Point(0, 1), Point(1,0), Point(0,6),Point(1,7),
        Point(6, 0), Point(7,1), Point(7,6),Point(6,7)
    }
};
std::array<Point, 4> x_spots{{
        Point(1, 1), Point(1,6), Point(6,1),Point(6,6)
    }
};
std::array<Point, 8> dir{{
        Point(1, 0), Point(0,1),
        Point(-1, 0), Point(0,1),
        Point(1, 0), Point(0,-1),
        Point(-1,0), Point(0,-1)
    }
};

Point ans(0,0);
map<int,Point> h_map;

class myOthello{
    public:
        enum SPOT_STATE {
            EMPTY = 0,
            BLACK = 1,
            WHITE = 2
        };
        static const int SIZE = 8;
        const std::array<Point, 8> directions{{
            Point(-1, -1), Point(-1, 0), Point(-1, 1),
            Point(0, -1), Point(0, 1),
            Point(1, -1), Point(1, 0), Point(1, 1)
        }};

        std::array<std::array<int, SIZE>, SIZE> board;
        std::vector<Point> next_valid_spots;
        std::array<int, 3> disc_count;
        int cur_player;
        bool done;
        int winner;
        int heuristic;
        int next_x,next_y;

    private:
        int get_next_player(int player) const {
            return 3 - player;
        }
        bool is_spot_on_board(Point p) const {
            return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
        }
        int get_disc(Point p) const {
            return board[p.x][p.y];
        }
        void set_disc(Point p, int disc) {
            board[p.x][p.y] = disc;
        }
        
        // neighbor spot should exist a enemy
        bool is_disc_at(Point p, int disc) const {
            if (!is_spot_on_board(p))
                return false;
            if (get_disc(p) != disc)
                return false;
            return true;
        }
        // valid to put my disk or not
        bool is_spot_valid(Point center) const {
            if (get_disc(center) != EMPTY){
                cout << "surprisingly not empty\n";
                return false;
            }
            for (Point dir: directions) {
                // Move along the direction while testing.
                Point p = center + dir;
                if (!is_disc_at(p, get_next_player(cur_player)))
                    continue;
                p = p + dir;
                while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                    if (is_disc_at(p, cur_player))
                        return true;
                    p = p + dir;
                }
            }
            return false;
        }
        void flip_discs(Point center) {
            for (Point dir: directions) {
                // Move along the direction while testing.
                Point p = center + dir;
                if (!is_disc_at(p, get_next_player(cur_player)))
                    continue;
                std::vector<Point> discs({p});
                p = p + dir;
                while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                    if (is_disc_at(p, cur_player)) {
                        for (Point s: discs) {
                            set_disc(s, cur_player);
                        }
                        disc_count[cur_player] += discs.size();
                        disc_count[get_next_player(cur_player)] -= discs.size();
                        break;
                    }
                    discs.push_back(p);
                    p = p + dir;
                }
            }
        }
    
    public:
        myOthello(){
            reset();
        }
        // copy assignment
        myOthello& operator=(const myOthello& rhs){
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    board[i][j] = rhs.board[i][j];
                }
            }
            disc_count[EMPTY] = rhs.disc_count[EMPTY];
            disc_count[BLACK] = rhs.disc_count[BLACK];
            disc_count[WHITE] = rhs.disc_count[WHITE];
            
            next_valid_spots = rhs.get_valid_spots();
            cur_player = rhs.cur_player;
            done = false;
            winner = -1;
            heuristic = rhs.heuristic;

            return *this;
        }// end function

        // set by input array
        void set(std::array<std::array<int, SIZE>, SIZE> input){
            disc_count[BLACK] = 0;
            disc_count[WHITE] = 0;
            disc_count[EMPTY] = 0;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    board[i][j] = input[i][j];
                    if(board[i][j] == 1) disc_count[BLACK]++;
                    else if(board[i][j] == 2) disc_count[WHITE]++;
                    else disc_count[EMPTY]++;
                }
            }
            next_valid_spots = get_valid_spots();
            done = false;
            winner = -1;
        }

        void reset() {
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    board[i][j] = EMPTY;
                }
            }
            board[3][4] = board[4][3] = BLACK;
            board[3][3] = board[4][4] = WHITE;
            cur_player = BLACK;
            disc_count[EMPTY] = 8*8-4;
            disc_count[BLACK] = 2;
            disc_count[WHITE] = 2;
            next_valid_spots = get_valid_spots();
            done = false;
            winner = -1;
            heuristic = 0;
        }

        std::vector<Point> get_valid_spots() const {
            std::vector<Point> valid_spots;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    Point p = Point(i, j);
                    if (board[i][j] != EMPTY)
                        continue;
                    if (is_spot_valid(p))
                        valid_spots.push_back(p);
                }
            }
            return valid_spots;
        }
        
        bool put_disc(Point p) {
            if(!is_spot_valid(p)) {
                winner = get_next_player(cur_player);
                done = true;
                return false;
            }
            set_disc(p, cur_player);
            disc_count[cur_player]++;
            disc_count[EMPTY]--;
            flip_discs(p);
            // Give control to the other player.
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            // Check Win
            if (next_valid_spots.size() == 0) {
                cur_player = get_next_player(cur_player);
                next_valid_spots = get_valid_spots();
                if (next_valid_spots.size() == 0) {
                    // Game ends
                    done = true;
                    int white_discs = disc_count[WHITE];
                    int black_discs = disc_count[BLACK];
                    if (white_discs == black_discs) winner = EMPTY;
                    else if (black_discs > white_discs) winner = BLACK;
                    else winner = WHITE;
                }
            }
            return true;
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

bool on_board(Point p){
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}

int heuristic(myOthello cur){
    bool maximizer = cur.cur_player==1;
    int heuristic = 0;
    int bk = 0, wh = 0;

    // corners: 3000
    const int weight_corners = 3000;
    for(auto i:corners){
        if( cur.board[i.x][i.y] == 1)
            bk ++;
        else if(cur.board[i.x][i.y] == 2)
            wh ++;
    }
    heuristic += weight_corners*(bk-wh);

    // x-squares: 900
    const int weight_x = 900;
    for(int i = 0; i < 4; i ++){
        Point CO = corners[i];
        Point X = x_spots[i];
        if( !cur.board[X.x][X.y] && cur.board[X.x][X.y]!= cur.board[CO.x][CO.y] ){
            if(cur.board[X.x][X.y] == 1)
                heuristic -= weight_x;
            else heuristic += weight_x;
        }
    }

    // c-squares: 1000
    const int weight_c = 1000;
    for(int i = 0; i < 4; i ++){
        Point CO = corners[i];
        for(int j = 0; j < 2; j ++){
            Point C = c_spots[2*i+j];
            if( !cur.board[C.x][C.y] && cur.board[C.x][C.y] != cur.board[CO.x][CO.y] ){
                if(cur.board[C.x][C.y] == 1)
                    heuristic -= weight_c;
                else heuristic += weight_c;
            }
            else if( !cur.board[C.x][C.y] && cur.board[C.x][C.y] == cur.board[CO.x][CO.y] ){
                if(cur.board[C.x][C.y] == 1)
                    heuristic += weight_c;
                else heuristic -= weight_c;
            }
        }
    }

    // 角 連邊: 900
    const int weight_line = 900;
    int bk_linecount = 0;
    int wh_linecount = 0;
    for(int i = 0; i < 4; i ++){
        /*
        Point(0, 0), Point(0,7), Point(7,0),Point(7,7)

        Point(1, 0), Point(0,1),
        Point(-1, 0), Point(0,1),
        Point(1, 0), Point(0,-1),
        Point(-1,0), Point(0,-1)
        */
        int color;
        Point c = corners[i];
        if(!cur.board[c.x][c.y]) continue;
        else color = cur.board[c.x][c.y];
        for(int j = 0; j < 2; j ++){
            Point next = c + dir[2*i+j];
            while(cur.board[next.x][next.y] == color && on_board(next)){
                if(color == 1) bk_linecount++;
                else wh_linecount++;
                next = next + dir[2*i+j];
            }
        }
    }
    heuristic += weight_line*(bk_linecount-wh_linecount);

    // Mobility: 30
    const int weight_mobility = 30;
    if(maximizer)
        heuristic += weight_mobility* cur.next_valid_spots.size();
    else heuristic += -weight_mobility* cur.next_valid_spots.size();

    // total num: 200 
    const int weight_total = 200;
    heuristic += weight_total*(cur.BLACK-cur.WHITE);

    cur.heuristic = heuristic;
    return heuristic;
}

int abprune(myOthello curnode, int depth, int alpha, int beta){
    bool maximizer = curnode.cur_player==1;

    if(depth == 0 || curnode.done){
        return heuristic(curnode);
    }
    
    if(maximizer){
        int maxeval = INT32_MIN;
        for(auto i:curnode.next_valid_spots){
            myOthello next = curnode;
            if(!next.put_disc(i)){
                cout << "hi\n";
                continue;
            }
            else{
                int eval = abprune( next,depth-1,alpha,beta); 
                maxeval = max(maxeval,eval);
                if(depth == MaxDepth)
                    h_map.insert(pair<int,Point>(eval,i));

                alpha = max(alpha,eval);
                if(beta <= alpha)
                    break;
            }
        }
        curnode.heuristic = maxeval;
        return maxeval;
    }

    else{
        int mineval = INT32_MAX;
        for(auto i:curnode.next_valid_spots){
            myOthello next = curnode;
            if(!next.put_disc(i)){
                cout << "hi\n";
                continue;
            }
            else{
                int eval = abprune( next,depth-1,alpha,beta);
                mineval = min(mineval,eval);
                if(depth == MaxDepth)
                    h_map.insert(pair<int,Point>(eval,i));

                beta = min(beta,eval);
                if(beta <= alpha) 
                    break;
            }
        }
        curnode.heuristic = mineval;
        return mineval;
    }

} // end function

void write_valid_spot(std::ofstream& fout) {
    bool searched = false;
    int n_valid_spots = next_valid_spots.size();
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_valid_spots[index];
    srand(time(NULL));
    // ===================================
    // find good moves here
    // black =1  =maximizer
    myOthello cur;
    cur.set(board);
    cur.cur_player = player;
    cur.next_valid_spots = next_valid_spots;
    cur.heuristic = abprune(cur,MaxDepth, INT32_MIN, INT32_MAX);
    
    for(auto i:h_map){
        //cout << "map: " << i.second.x << "," << i.second.y << endl;
        if(i.first == cur.heuristic){
            p.x = i.second.x;
            p.y = i.second.y;
            searched = true;
            break;
        }
    }
    // ===================================
    // Remember to flush the output to ensure the last action is written to file.
    if(!searched) cout << "no searched\n";
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
