#ifndef _2D_H_
#define _2D_H_

#include <stdlib.h>

template< typename Value > class TwoDim {
public:
    TwoDim() : width_(0), height_(0), buf_(NULL) {}
    TwoDim(int width, int height, Value value) : width_(width), height_(height) {
        new_buf();
        clear(value);
    }
    void clear(Value value) {
        int tlen = len();
        for (int i = 0; i < tlen; ++i)
            buf_[i] = value;
    }
#if 0
    void clear(char value) {
        memset(buf_, value, len());
    }
#endif
    void resize(int width, int height) {
        if (width == width_ && height == height_) return;
        if (buf_) delete[] buf_;
        width_ = width;
        height_ = height;
        new_buf();
    }
    Value get(int x, int y) const { return buf_[index(x,y)]; }
    Value set(int x, int y, Value value) { buf_[index(x,y)] = value; return get(x,y); }
private:
    void new_buf() { 
        buf_ = new Value[len()];
    }
    int index(int x, int y) const {
        if (x >= width_ || y >= height_) abort();
        return y * width_ + x;
    }
    int len() const {
        return width_ * height_;
    }
    int width_;
    int height_;
    Value* buf_;
};

#endif // _2D_H_
