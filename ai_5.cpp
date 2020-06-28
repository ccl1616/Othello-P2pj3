#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;
int player;
const int SIZE = 8;
struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};
int eff[SIZE][SIZE]{
{-3,-4,3,3,3,3,-4,-3},
{-4,-5,5,5,5,5,-5,-4},
{ 3, 5,5,5,5,5, 5, 3},
{ 3, 5,5,5,5,5, 5, 3},
{ 3, 5,5,5,5,5, 5, 3},
{ 3, 5,5,5,5,5, 5, 3},
{-4,-5,5,5,5,5,-5,-4},
{-3,-4,3,3,3,3,-4,-3}
};
class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    array<array<int, SIZE>, SIZE> board;
    vector<Point> next_valid_spots;
    array<int, 3> disc_count;
    int cur_player;
    bool done;

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

    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
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
public:
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
    OthelloBoard() {
       for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        disc_count[EMPTY] = 62;
        disc_count[BLACK] = 1;
        disc_count[WHITE] = 1;
    }

    OthelloBoard(const OthelloBoard &ref){
       for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
       for (int i = 0; i < 3 ; i++)
        disc_count[i] = ref.disc_count[i];
    }
    vector<Point> get_valid_spots() const {
        vector<Point> valid_spots;
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
    void set_valid_spots(){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    next_valid_spots.push_back(p);
            }
        }
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    int Heuristic(){
        int H = 0;
        OthelloBoard opponent = *this;
        opponent.cur_player = 3 - player;
        opponent.set_valid_spots();

        //disc count
        H += disc_count[player] - disc_count[3 - player];
        //value for map
        for(auto np : next_valid_spots)
            H += 10*eff[np.x][np.y];
        //legal move count
        H += (next_valid_spots.size() - opponent.next_valid_spots.size())* 100;
        //corner move count
        for(auto np : next_valid_spots)
            if(np == Point(0,0) || np == Point(0,7) || np == Point(7,0) || np == Point(7,7))
            H += 900;
        if(opponent.next_valid_spots.size() == 0)
            H += 100000;
        return H;
    }
};
OthelloBoard cur_board;


void read_board(ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> cur_board.board[i][j];
        }
    }
}

void read_valid_spots(ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    float x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        cur_board.next_valid_spots.push_back({x, y});
    }
}
int alpha_beta(OthelloBoard cur,int depth,int a, int b,int player){
    cur.set_valid_spots();
    if(depth == 0 || cur.next_valid_spots.size() == 0)
        return cur.Heuristic();
    if(player == 1){
        for(auto np : cur.next_valid_spots){
            OthelloBoard next = cur;
            next.flip_discs(np);
            next.set_disc(np,player);
            a = max(a,alpha_beta(next,depth - 1, a , b , 3 - player));
            if(b <= a)
                break;
        }
        return a;
    }
    else if (player == 2){
        for(auto np : cur.next_valid_spots){
            OthelloBoard next = cur;
            next.flip_discs(np);
            next.set_disc(np,player);
            b = min(b,alpha_beta(next,depth - 1, a , b , 3 - player));
            if(b <= a)
                break;
        }
        return b;
    }
    else return -1;
}
void write_valid_spot(ofstream& fout) {
    //bad state function -- greedy
    Point p(0,0);
        int min_num = -2147483647;
        for(auto np : cur_board.next_valid_spots){
            if(np == Point(0,0) || np == Point(0,7) || np == Point(7,0) || np == Point(7,7)){
                p = np;
                break;
            }
            OthelloBoard next = cur_board;
            next.flip_discs(np);
            next.set_disc(np,player);
            int next_a = alpha_beta(next,5,-2147483648,2147483647, 3 - player);
            if(next_a > min_num){
                min_num = next_a;
                p = np;
            }
    }
    //minimax
    //alpha beta pruning
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

/* alpha-beta template
function alphabeta(node, depth, a, b, maximizingPlayer)
     if depth = 0 or node is a terminal node
         return the heuristic value of node
     if maximizingPlayer
         for each child of node
             a := max(a, alphabeta(child, depth - 1, a, b, FALSE))
             if b <= a
                 break (* b cut-off *)
         return a
     else
         for each child of node
             b := min(b, alphabeta(child, depth - 1, a, b, TRUE))
             if b <= a
                 break (* a cut-off *)
         return b
*/


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
