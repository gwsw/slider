#ifndef _BOARD_H_
#define _BOARD_H_

#include <deque>
#include "piece.h"

#define MAX_WIDTH 100
#define MAX_HEIGHT 100

class Board {
public:
    Board(int width, int height) : width_(width), height_(height), goal_pix_(-1), xgoal_(-1), ygoal_(-1) {
        clear();
    }
    void clear() {
        //memset
        for (int x = 0; x < width_; ++x) {
            for (int y = 0; y < height_; ++y) {
                grid_[x][y] = '\0';
            }
        }
    }
    bool set_goal(int pix, int xgoal, int ygoal) {
        goal_pix_ = pix;
        xgoal_ = xgoal;
        ygoal_ = ygoal;
        return true;
    }
    void reset() {
        clear();
        for (auto ipiece = pieces_.begin(); ipiece != pieces_.end(); ++ipiece) {
            ipiece->reset();
            (void) set_piece(*ipiece);
        }
    }
    bool set_piece(Piece& piece) {
        if (piece.orient == Piece::HORZ) {
            for (int x = 0; x < piece.len; ++x) {
                if (piece.x+x >= width_ || grid_[piece.x+x][piece.y]) {
                    for (int rx = 0; rx < x; ++rx)
                        grid_[piece.x+rx][piece.y] = '\0';
                    return false;
                }
                grid_[piece.x+x][piece.y] = piece.name;
            }
        } else {
            for (int y = 0; y < piece.len; ++y) {
                if (piece.y+y >= height_ || grid_[piece.x][piece.y+y]) {
                    for (int ry = 0; ry < y; ++ry)
                        grid_[piece.x][piece.y+ry] = '\0';
                    return false;
                }
                grid_[piece.x][piece.y+y] = piece.name;
            }
        }
        return true;
    }
    bool add_piece(Piece& piece) {
        if (!set_piece(piece)) return false;
        pieces_.insert(pieces_.end(), piece);
        return true;
    }
    bool move(int pix, bool fwd) {
        if (pix < 0 || pix >= (int) pieces_.size()) return false;
        Piece& piece = pieces_[pix];
        if (fwd) {
            if (piece.orient == Piece::HORZ) {
                int next = piece.x + piece.len;
                if (next >= width_ || grid_[next][piece.y]) return false;
                grid_[next][piece.y] = piece.name;
                grid_[piece.x][piece.y] = '\0';
                piece.x++;
            } else {
                int next = piece.y + piece.len;
                if (next >= height_ || grid_[piece.x][next]) return false;
                grid_[piece.x][next] = piece.name;
                grid_[piece.x][piece.y] = '\0';
                piece.y++;
            }
        } else {
            if (piece.orient == Piece::HORZ) {
                int next = piece.x - 1;
                if (next < 0 || grid_[next][piece.y]) return false;
                grid_[next][piece.y] = piece.name;
                grid_[piece.x+piece.len-1][piece.y] = '\0';
                piece.x--;
            } else {
                int next = piece.y - 1;
                if (next < 0 || grid_[piece.x][next]) return false;
                grid_[piece.x][next] = piece.name;
                grid_[piece.x][piece.y+piece.len-1] = '\0';
                piece.y--;
            }
        }
        return true;
    }
    bool is_won() const {
        if (goal_pix_ < 0) return false;
        Piece const& piece = pieces_[goal_pix_];
        if (piece.orient == Piece::HORZ) {
            for (int x = piece.x + piece.len; x < width_; ++x)
                if (grid_[x][piece.y] != '\0') return false;
        } else {
            for (int y = piece.y + piece.len; y < width_; ++y)
                if (grid_[piece.x][y] != '\0') return false;
        }
        return true;
    }
    char piece_name(int pix) const {
        return pieces_[pix].name;
    }
    int num_pieces() const {
        return (int) pieces_.size();
    }
    void dump() const {
        printf(" "); for (int x = 0; x < width_; ++x) { printf("-"); } printf("\n");
        for (int y = 0; y < height_; ++y) {
            printf("|");
            for (int x = 0; x < width_; ++x) {
                printf("%c", grid_[x][y] ? grid_[x][y] : '.');
            }
            printf("|\n");
        }
        printf(" "); for (int x = 0; x < width_; ++x) { printf("-"); } printf("\n");
    }
private:
    int width_;
    int height_;
    char grid_[MAX_WIDTH][MAX_HEIGHT];
    std::deque<Piece> pieces_;
    int goal_pix_;
    int xgoal_;
    int ygoal_;
};

#endif // _BOARD_H_
