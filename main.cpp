#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "solve.h"
#include "mgr.h"

char Piece::next_name_;

bool verbose = false;
bool print_all = false;
bool print_asap = false;
bool show_moves = false;
bool print_progress = false;
bool dump_solns = false;
bool draw_solns = false;
bool print_color = false;
int num_threads = 1;

static int usage() {
    printf("usage: slider [-options] [commands...]\n");
    printf("  Options:\n");
    printf("       -a       Print all solutions\n");
    printf("       -A       Print solutions ASAP\n");
    printf("       -c       Draw boards with color\n");
    printf("       -d       Dump boards (one char per cell)\n");
    printf("       -D       Draw boards (line art)\n");
    printf("       -f FILE  Board description in file\n");
    printf("       -m       Show solution moves\n");
    printf("       -p       Print progress\n");
    printf("       -t NUM   Number of solution threads\n");
    printf("       -v       Verbose\n");
    printf("  Commands:\n");
    printf("       h X,Y,L  Add L-length horizontal piece at (x,y)\n");
    printf("       v X,Y,L  Add L-length vertical piece at (x,y)\n");
    printf("       g P,X,Y  Set goal piece to P, goal cell at (x,y)\n");
    printf("       m [P...] Move piece(s). Seq of pieces each optionally followed by ' to reverse\n");
    printf("       s NUM    Solve board, max NUM moves\n");
    printf("       k        Dump board\n");
    printf("       D        Draw board\n");
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

static bool move_pieces(char* cmd, Board& board) {
    while (*cmd != '\0') {
        if (*cmd == ' ' || *cmd == '\n' || *cmd == '\r') {
            ++cmd;
            continue;
        }
        char* next;
        int pix = strtoul(cmd, &next, 0);
        if (cmd == next) {
            pix = board.piece_pix(*cmd);
            if (pix < 0) {
                printf("invalid piece at \"%s\"\n", cmd);
                return false;
            }
            ++cmd;
        }
        bool fwd = (*cmd != '\'');
        if (!fwd) ++cmd;
        if (!board.move(pix, fwd)) {
            printf("invalid move before \"%s\"\n", cmd);
            return false;
        }
    }
    return true;
}

static void solve_progress(void* ctx, int pct) {
    printf("  %d%%   \r", pct);
    fflush(stdout);
}

static bool solve_moves(Board& board, int num_moves) {
    Mgr mgr (board, num_moves, print_progress ? &solve_progress : NULL, NULL, num_threads);
    return mgr.run();
}

static bool solve(char* cmd, Board& board) {
    if (*cmd != '\0') {
        int num_moves = strtoul(cmd, &cmd, 0);
        return solve_moves(board, num_moves);
    } else {
        for (int num_moves = 1;; ++num_moves) {
            if (solve_moves(board, num_moves))
                return true;
        }
    }
}

static bool cmd(char* cmd, Board& board) {
    if (board.is_won()) printf("*** WON!\n");
    switch (cmd[0]) {
    case 'd': board.dump(); return true;
    case 'D': board.draw(); return true;
    case 'v': return add_piece(&cmd[1], Piece::VERT, board); 
    case 'h': return add_piece(&cmd[1], Piece::HORZ, board);
    case 'g': return set_goal(&cmd[1], board);
    case 'm': return move_pieces(&cmd[1], board);
    case 's': return solve(&cmd[1], board);
    default: printf("unknown cmd %s\n", cmd); return false;
    }
}

int main(int argc, char** argv) {
    int width = 0;
    int height = 0;
    char* board_file = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "aAcdDf:h:mpt:w:v")) != -1) {
        switch (opt) {
        case 'a': print_all = true; break;
        case 'A': print_asap = true; break;
        case 'c': print_color = true; break;
        case 'd': dump_solns = true; break;
        case 'D': draw_solns = true; break;
        case 'f': board_file = optarg; break;
        case 'h': height = strtol(optarg, NULL, 10); break;
        case 'm': show_moves = true; break;
        case 'p': print_progress = true; break;
        case 't': num_threads = strtol(optarg, NULL, 10); break;
        case 'w': width = strtol(optarg, NULL, 10); break;
        case 'v': verbose = true; break;
        default: return usage();
        }
    }
    Board* board = NULL;
    if (board_file) {
        FILE* fd = fopen(board_file, "r");
        if (fd == NULL) {
            printf("cannot open %s\n", board_file);
            return 1;
        }
        board = Board::read_file(fd);
        fclose(fd);
    } else if (width > 0 && height > 0) {
        board = new Board(width, height);
    }
    if (board == NULL) {
        printf("ERROR: no board\n");
        return 1;
    }
    for (int arg = optind; arg < argc; ++arg) {
        if (!cmd(argv[arg], *board)) printf("cmd failed\n");
    }
    delete board;
    return 0;
}
