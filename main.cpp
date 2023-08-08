#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "board.h"

char Piece::next_name_ = 'A';

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
    board.add_piece(piece);
    return true;
}

static bool set_goal(char* cmd, Board& board) {
    int pix = strtoul(cmd, &cmd, 0);
    if (*cmd++ != ',') return false;
    int x = strtoul(cmd, &cmd, 0);
    if (*cmd++ != ',') return false;
    int y = strtoul(cmd, &cmd, 0);
    return board.set_goal(pix, x, y);
}

static void cmd(char* cmd, Board& board) {
    switch (cmd[0]) {
    case 'p': board.dump(); if (board.is_won()) printf("WON!\n"); break;
    case 'v': add_piece(&cmd[1], Piece::VERT, board); break;
    case 'h': add_piece(&cmd[1], Piece::HORZ, board); break;
    case 'g': set_goal(&cmd[1], board); break;
    case 'm': {
        int pix = strtoul(cmd+1, &cmd, 0);
        if (!board.move(pix, cmd[0] == 'f')) printf("cannot move %d\n", pix);
        break; }
    default: printf("unknown cmd %s\n", cmd); break;
    }
}

int main(int argc, char** argv) {
    int width = 5;
    int height = 5;
    int opt;
    while ((opt = getopt(argc, argv, "h:w:")) != -1) {
        switch (opt) {
        case 'h': height = strtol(optarg, NULL, 10); break;
        case 'w': width = strtol(optarg, NULL, 10); break;
        default: return usage();
        }
    }
    Board board(width, height);
    for (int arg = optind; arg < argc; ++arg) {
        cmd(argv[arg], board);
    }
    return 0;
}
