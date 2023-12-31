#ifndef _BOARD_H_
#define _BOARD_H_

#include <string.h>
#include <deque>
#include <map>
#include "piece.h"
#include "2d.h"

extern bool print_color;

#define MAX_WIDTH 100
#define MAX_HEIGHT 100

class Board {
public:
    Board(int width, int height) : width_(width), height_(height), grid_(width,height,'\0'), key_pix_(-1) {
    }
    int get_width() const {
        return width_;
    }
    int get_height() const {
        return height_;
    }
    void clear() {
        grid_.clear('\0');
    }
    bool set_goal(int pix) {
        key_pix_ = pix;
        return true;
    }
    void reset() {
        clear();
        for (auto ipiece = pieces_.begin(); ipiece != pieces_.end(); ++ipiece) {
            ipiece->reset();
            (void) set_piece(*ipiece);
        }
    }
    static Board* read_file(FILE* fd) {
        std::map<char,bool> did_name;
        int width = 0;
        int height = 0;
        char key_name = '\0';
        Board* board = NULL;
        TwoDim<char> pic;
        for (int y = 0; height == 0 || y < height; ++y) {
            int len = 0;
            char line[256];
            if (fgets(line, sizeof(line), fd) != NULL)
                len = strcspn(line, "\r\n");
            if (len > 2 && line[0] == '#') {
                char* p;
                switch (line[1]) {
                case 'w': width = strtoul(&line[2], NULL, 0); break;
                case 'h': height = strtoul(&line[2], NULL, 0); break;
                case 'b':
                    width = strtoul(&line[2], &p, 0);
                    if (*p != ',' && *p != 'x') {
                        printf("ERROR: invalid #b line in board file\n");
                        delete board;
                        return NULL;
                    }
                    height = strtoul(p+1, NULL, 0);
                    break;
                case 'g':
                    if (len < 5) {
                        printf("ERROR: invalid #g line in board file\n");
                        delete board;
                        return NULL;
                    }
                    for (p = &line[2]; *p == ' '; ++p)
                        ;
                    key_name = *p++;
                    break;
                default: break;
                }
                if (board == NULL && width > 0 && height > 0) {
                    board = new Board(width, height);
                    pic.resize(width, height, '\0');
                }
                --y;
                continue;
            }
            if (board == NULL) {
                printf("ERROR: missing width/height in board file\n");
                return NULL;
            }
            for (int x = 0; x < width; ++x) {
                char ch = (x < len) ? line[x] : ' ';
                pic.set(x, y, ch);
                if (ch != ' ' && ch != '.')
                    did_name[ch] = true;
            }
        }
        if (board == NULL || key_name == '\0') {
            printf("ERROR: incomplete board file\n");
            return NULL;
        }
        for (auto inm = did_name.begin(); inm != did_name.end(); ++inm) {
            if (!create_piece_from_file(board, inm->first, pic)) {
                delete board;
                return NULL;
            }
        }
        int key_pix = board->piece_pix(key_name);
        if (key_pix < 0) {
            printf("ERROR: missing key piece %c\n", key_name);
            delete board;
            return NULL;
        }
        if (!board->set_goal(key_pix)) {
            printf("ERROR: invalid goal\n");
            delete board;
            return NULL;
        }
        return board;
    }
    static bool create_piece_from_file(Board* board, char name, TwoDim<char> const& pic) {
        for (int x = 0; x < board->width_; ++x) {
            for (int y = 0; y < board->height_; ++y) {
                if (pic.get(x, y) == name) {
                    Piece::Orient orient;
                    int len;
                    if (x+1 < board->width_ && pic.get(x+1, y) == name) {
                        int ix;
                        for (ix = x+2; ix < board->width_; ++ix)
                            if (pic.get(ix, y) != name)
                                break;
                        len = ix-x;
                        orient = Piece::HORZ;
                    } else {
                        int iy;
                        for (iy = y+1; iy < board->height_; ++iy)
                            if (pic.get(x, iy) != name)
                                break;
                        len = iy-y;
                        orient = Piece::VERT;
                    }
                    Piece piece (x, y, len, orient, name);
                    if (!board->add_piece(piece)) {
                        printf("cannot add piece at (%d,%d)\n", x, y);
                    }
                    return true;
                }
            }
        }
        return false;
    }
    bool set_piece(Piece& piece) {
        if (piece.orient == Piece::HORZ) {
            for (int x = 0; x < piece.len; ++x) {
                if (piece.x+x >= width_ || grid_.get(piece.x+x, piece.y)) {
                    for (int rx = 0; rx < x; ++rx)
                        grid_.set(piece.x+rx, piece.y, '\0');
                    return false;
                }
                grid_.set(piece.x+x, piece.y, piece.name);
            }
        } else {
            for (int y = 0; y < piece.len; ++y) {
                if (piece.y+y >= height_ || grid_.get(piece.x, piece.y+y)) {
                    for (int ry = 0; ry < y; ++ry)
                        grid_.set(piece.x, piece.y+ry, '\0');
                    return false;
                }
                grid_.set(piece.x, piece.y+y, piece.name);
            }
        }
        return true;
    }
    bool add_piece(Piece& piece) {
        if (!set_piece(piece)) return false;
        pieces_.push_back(piece);
        return true;
    }
    bool move(int pix, bool fwd) {
        if (pix < 0 || pix >= (int) pieces_.size()) return false;
        Piece& piece = pieces_[pix];
        if (fwd) {
            if (piece.orient == Piece::HORZ) {
                int next = piece.x + piece.len;
                if (next >= width_ || grid_.get(next, piece.y)) return false;
                grid_.set(next, piece.y, piece.name);
                grid_.set(piece.x, piece.y, '\0');
                piece.x++;
            } else {
                int next = piece.y + piece.len;
                if (next >= height_ || grid_.get(piece.x, next)) return false;
                grid_.set(piece.x, next, piece.name);
                grid_.set(piece.x, piece.y, '\0');
                piece.y++;
            }
        } else {
            if (piece.orient == Piece::HORZ) {
                int next = piece.x - 1;
                if (next < 0 || grid_.get(next, piece.y)) return false;
                grid_.set(next, piece.y, piece.name);
                grid_.set(piece.x+piece.len-1, piece.y, '\0');
                piece.x--;
            } else {
                int next = piece.y - 1;
                if (next < 0 || grid_.get(piece.x, next)) return false;
                grid_.set(piece.x, next, piece.name);
                grid_.set(piece.x, piece.y+piece.len-1, '\0');
                piece.y--;
            }
        }
        return true;
    }
    bool is_won() const {
        if (key_pix_ < 0) return false;
        Piece const& piece = pieces_[key_pix_];
        if (piece.orient == Piece::HORZ) {
            for (int x = piece.x + piece.len; x < width_; ++x)
                if (grid_.get(x, piece.y) != '\0') return false;
        } else {
            for (int y = piece.y + piece.len; y < width_; ++y)
                if (grid_.get(piece.x, y) != '\0') return false;
        }
        return true;
    }
    char piece_name(int pix) const {
        return pieces_[pix].name;
    }
    int piece_pix(char name) const {
        for (int pix = 0; pix < (int) pieces_.size(); ++pix)
            if (pieces_[pix].name == name)
                return pix;
        return -1;
    }
    int num_pieces() const {
        return (int) pieces_.size();
    }
    void dump() const {
        printf(" "); for (int x = 0; x < width_; ++x) { printf("-"); } printf("\n");
        for (int y = 0; y < height_; ++y) {
            printf("|");
            for (int x = 0; x < width_; ++x) {
                char ch = grid_.get(x, y);
                printf("%c", ch ? ch : '.');
            }
            printf("|\n");
        }
        printf(" "); for (int x = 0; x < width_; ++x) { printf("-"); } printf("\n");
    }
    void draw() const {
        draw_top();
        for (int y = 0; y < height_; ++y) {
            draw_horz(y);
            draw_vert(y);
        }
        draw_horz(height_);
        draw_bot();
    }

private:
    void draw_top() const {
        printf("*");
        for (int x = 0; x < width_; ++x)
            printf(" * * *");
        printf(" *\n");
    }
    void draw_bot() const {
        draw_top();
    }
    void draw_horz(int y) const {
        printf("*");
        for (int x = 0; x < width_; ++x) {
            if (grid_get(x,y) != grid_get(x,y-1)) {
                draw_horz_bar(x, y, '-');
            } else {
                draw_horz_bar(x, y, ' ', grid_get(x,y));
            }
        }
        printf("%c*\n", grid_get(width_-1,y)!=grid_get(width_-1,y-1) ? '+' : grid_get(width_-1,y) ? '|' : ' ');
    }
    void draw_vert(int y) const {
        for (int line = 0; line < 2; ++line) {
            printf("*");
            for (int x = 0; x < width_; ++x) {
                if (grid_get(x,y) != grid_get(x-1,y)) {
                    draw_vert_bar(x, y, '|');
                } else {
                    draw_vert_bar(x, y, ' ');
                }
            }
            printf("%c*\n", grid_get(width_-1,y) ? '|' : ' ');
        }
    }
    void draw_horz_bar(int x, int y, char chm, char color_ch = '\0') const {
        char bar = hchar(x-1, y-1);
        printf("%c%s%c%c%c%c%c%s", bar, start_color(color_ch), chm, chm, color_ch ? color_ch : chm, chm, chm, end_color(color_ch));
    }
    void draw_vert_bar(int x, int y, char bar) const {
        char ch = grid_get(x,y);
        if (bar == ' ')
            printf("%s%c", start_color(ch), bar);
        else 
            printf("%c%s", bar, start_color(ch));
        printf("  %c  %s", ch ? ch : ' ', end_color(ch));
    }
    char hchar(int x, int y) const {
        return hchar(grid_get(x,y), grid_get(x+1,y), grid_get(x,y+1), grid_get(x+1,y+1));
    }
    char hchar(char ul, char ur, char ll, char lr) const {
        if (!ul && !ur && !ll && !lr) return ' ';
        if (ul == ur && ll == lr) return '-';
        if (ul == ll && ur == lr) return '|';
        if (ul || ur || ll || lr) return '+';
        return ' ';
    }
    char grid_get(int x, int y) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) return '\0';
        return grid_.get(x,y);
    }
    char const* start_color(char ch) const {
        if (!print_color || ch == ' ' || ch == '\0') return "";
        static char const* sgr[] = {
            "\e[40;37m",
            "\e[41;37m",
            "\e[42;30m",
            "\e[43;30m",
            "\e[44;37m",
            "\e[45;30m",
            "\e[46;30m",
            "\e[47;30m",
        };
        return sgr[ch % 8];
    }
    char const* end_color(int ch) const {
        if (!print_color || ch == ' ' || ch == '\0') return "";
        return "\e[m";
    }

private:
    int width_;
    int height_;
    TwoDim<char> grid_;
    std::deque<Piece> pieces_;
    int key_pix_;
};

#endif // _BOARD_H_
