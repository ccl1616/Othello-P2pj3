#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <cassert>
#include "function.hpp"

const std::string file_log = "gamelog.txt";
const std::string file_state = "state";
const std::string file_action = "action";
// Timeout is set to 10 when TA test your code.
const int timeout = 10;

void launch_executable(std::string filename) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    std::string command = "start /min " + filename + " " + file_state + " " + file_action;
    std::string kill = "timeout /t " + std::to_string(timeout) + " > NUL && taskkill /im " + filename + " > NUL";
    system(command.c_str());
    system(kill.c_str());
#elif __linux__
    std::string command = "timeout " + std::to_string(timeout) + "s " + filename + " " + file_state + " " + file_action;
    system(command.c_str());
#elif __APPLE__
    // May require installing the command by:
    // brew install coreutils
    std::string command = "gtimeout " + std::to_string(timeout) + "s " + filename + " " + file_state + " " + file_action;
    system(command.c_str());
#endif
}

int main(int argc, char** argv) {
    assert(argc == 3);
    std::ofstream log("gamelog.txt");
    std::string player_filename[3];
    player_filename[1] = argv[1];
    player_filename[2] = argv[2];
    std::cout << "Player Black File: " << player_filename[OthelloBoard::BLACK] << std::endl;
    std::cout << "Player White File: " << player_filename[OthelloBoard::WHITE] << std::endl;
    OthelloBoard game;

    std::string data;
    data = game.encode_output();
    std::cout << data;
    log << data;

    while (!game.done) {
        // Output current state
        data = game.encode_state();
        std::ofstream fout(file_state);
        fout << data;
        fout.close();
        // Run external program
        launch_executable(player_filename[game.cur_player]);
        // Read action
        std::ifstream fin(file_action);
        Point p(-1, -1);
        while (true) {
            int x, y;
            if (!(fin >> x)) break;
            if (!(fin >> y)) break;
            p.x = x; p.y = y;
        }
        std::cout << "output: "<< p.x << "," << p.y << std::endl;
        fin.close();
        // Reset action file
        if (remove(file_action.c_str()) != 0)
            std::cerr << "Error removing file: " << file_action << "\n";
        // Take action
        if (!game.put_disc(p)) {
            // If action is invalid.
            data = game.encode_output(true);
            std::cout << data;
            log << data;
            break;
        }
        data = game.encode_output();
        std::cout << data;
        log << data;
    }
    log.close();
    // Reset state file
    if (remove(file_state.c_str()) != 0)
        std::cerr << "Error removing file: " << file_state << "\n";
    return 0;
}
