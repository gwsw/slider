#ifndef _2D_H_
#define _2D_H_

#include <stdlib.h>


template< typename Value > class TwoDim {
public:
    TwoDim() : width_(0), height_(0), buf_(NULL) {}
    TwoDim(int width, int height, Value value = Value()) : width_(width), height_(height) {
        new_buf();
        clear(value);
    }
    TwoDim(TwoDim const& orig) : width_(orig.width_), height_(orig.height_) {
        new_buf();
        int tlen = len();
        for (int i = 0; i < tlen; ++i)
            buf_[i] = orig.buf_[i];
    }
    template<class V> void clear(V value) {
        int tlen = len();
        for (int i = 0; i < tlen; ++i)
            buf_[i] = value;
    }
    void resize(int width, int height, Value value) {
        if (width == width_ && height == height_) return;
        if (buf_) delete[] buf_;
        width_ = width;
        height_ = height;
        new_buf();
        clear(value);
    }
    Value get(int x, int y) const { return buf_[index(x,y)]; }
    Value set(int x, int y, Value value) { buf_[index(x,y)] = value; return get(x,y); }
    int len() const { return width_ * height_; }

private:
    void new_buf() { 
        buf_ = new Value[len()];
    }
    int index(int x, int y) const {
        if (x >= width_ || y >= height_) abort();
        return y * width_ + x;
    }
    int width_;
    int height_;
    Value* buf_;
};

template<> template<> void TwoDim<char>::clear(char value) {
    memset(buf_, value, len());
}
#endif // _2D_H_
