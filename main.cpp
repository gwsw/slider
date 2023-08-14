#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "solve.h"

char Piece::next_name_;

bool verbose = false;
bool print_all = false;
bool show_moves = false;
bool print_progress = false;
bool dump_solns = false;
bool draw_solns = false;

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

static void solve_progress(void* ctx, int pct) {
    if (!print_progress) return;
    printf(" %d%%\r", pct);
    fflush(stdout);
}

static bool solve_moves(Board& board, int num_moves) {
    Solver solver(board);
    bool solved = solver.solve(num_moves, &solve_progress, NULL);
    if (!solved) printf("no %d move solution\n", num_moves);
    return solved;
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
    case 'm': return move_piece(&cmd[1], board);
    case 's': return solve(&cmd[1], board);
    default: printf("unknown cmd %s\n", cmd); return false;
    }
}

int main(int argc, char** argv) {
    int width = 0;
    int height = 0;
    char* board_file = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "adDf:h:mpw:v")) != -1) {
        switch (opt) {
        case 'a': print_all = true; break;
        case 'd': dump_solns = true; break;
        case 'D': draw_solns = true; break;
        case 'f': board_file = optarg; break;
        case 'h': height = strtol(optarg, NULL, 10); break;
        case 'm': show_moves = true; break;
        case 'p': print_progress = true; break;
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
    return 0;
}
