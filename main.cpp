#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "solve.h"

char Piece::next_name_ = 'A';
bool verbose = false;

static int usage() {
    printf("usage: slider [-w#][-h#] ...\n");
    return 1;
}

static bool add_piece(char* cmd, Piece::Orient orient, Board& board) {
    int x = strtoul(cmd, &cmd, 0);
    if (*cmd++ != ',') return false;
    int y = strtoul(cmd, &cmd, 0);
    if (*cmd++ != ',') return false;
    int len = strtoul(cmd, &cmd, 0);
    Piece piece (x, y, len, orient);
    return board.add_piece(piece);
}

static bool set_goal(char* cmd, Board& board) {
    int pix = strtoul(cmd, &cmd, 0);
    if (*cmd++ != ',') return false;
    int x = strtoul(cmd, &cmd, 0);
    if (*cmd++ != ',') return false;
    int y = strtoul(cmd, &cmd, 0);
    return board.set_goal(pix, x, y);
}

static bool move_piece(char* cmd, Board& board) {
    int pix = strtoul(cmd, &cmd, 0);
    return board.move(pix, cmd[0] == 'f');
}

static bool solve(char* cmd, Board& board) {
    int num_moves = strtoul(cmd, &cmd, 0);
    Solver solver(board);
    if (!solver.solve_moves(num_moves))
        printf("no solution\n");
    return true;
}

static bool cmd(char* cmd, Board& board) {
    switch (cmd[0]) {
    case 'p': board.dump(); if (board.is_won()) printf("WON!\n"); return true;
    case 'v': return add_piece(&cmd[1], Piece::VERT, board); 
    case 'h': return add_piece(&cmd[1], Piece::HORZ, board);
    case 'g': return set_goal(&cmd[1], board);
    case 'm': return move_piece(&cmd[1], board);
    case 's': return solve(&cmd[1], board);
    default: printf("unknown cmd %s\n", cmd); return false;
    }
}

int main(int argc, char** argv) {
    int width = 5;
    int height = 5;
    int opt;
    while ((opt = getopt(argc, argv, "h:w:v")) != -1) {
        switch (opt) {
        case 'h': height = strtol(optarg, NULL, 10); break;
        case 'w': width = strtol(optarg, NULL, 10); break;
        case 'v': verbose = true; break;
        default: return usage();
        }
    }
    Board board(width, height);
    for (int arg = optind; arg < argc; ++arg) {
        if (!cmd(argv[arg], board)) printf("cmd failed\n");
    }
    return 0;
}
