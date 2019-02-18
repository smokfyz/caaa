#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <time.h>

#define N 41

typedef unsigned char byte;

struct valley {
    byte l;
    byte r;
    byte done;
    byte pad;
    byte x;
    byte y;
};

struct found {
    int level;

    explicit found(int _level)
            : level(_level) {}
};

static valley valleys[N][N + 1];
static byte logs[N][2];

clock_t start;
clock_t stop;
clock_t all;

static void search(int level, int width, int bound);

// streamlined version of search() for the case width == bound
static void searchT(int level, int width)
{

    if (width == 0)
        throw found(level);

    for (int i = 0; i < width; i++) {
        if (valleys[level][i].l == valleys[level][i].r &&
            valleys[level][i].done < valleys[level][i].r)
        {
            byte w = valleys[level][i].l;
            for (int j = 0; j < i; j++) {
                valleys[level+1][j] = valleys[level][j];
                valleys[level+1][j].done =
                        std::min(valleys[level+1][j].l, valleys[level+1][j].r);
            }
            for (int j = i; j < width-1; j++)
                valleys[level+1][j] = valleys[level][j+1];
            logs[level][0] = i+1;
            logs[level][1] = w;
            if (i > 0)
                valleys[level+1][i-1].r += w;
            valleys[level+1][i].l += w;
            searchT(level+1, width-1);
        }
    }
}

// streamlined version of search() for width == bound-1
static void searchT1(int level, int width)
{

    if (width == 0)
        throw found(level);

    for (int i = 0; i < width; i++) {
        byte w = std::min(valleys[level][i].l, valleys[level][i].r);
        if (valleys[level][i].done < w) {
            for (int j = 0; j < i; j++) {
                valleys[level+1][j] = valleys[level][j];
                valleys[level+1][j].done =
                        std::min(valleys[level+1][j].l, valleys[level+1][j].r);
            }
            if (valleys[level][i].l == valleys[level][i].r) {
                if (i > 0)
                    valleys[level+1][i-1].r += w;
                valleys[level+1][i].l = valleys[level][i+1].l + w;
                valleys[level+1][i].r = valleys[level][i+1].r;
                valleys[level+1][i].y = valleys[level][i+1].y;
                valleys[level+1][i].x = valleys[level][i+1].x;
                valleys[level+1][i].done = valleys[level][i+1].done;
                for (int j = i+1; j < width-1; j++)
                    valleys[level+1][j] = valleys[level][j+1];
                logs[level][0] = i+1;
                logs[level][1] = w;
                searchT1(level+1, width-1);
            } else if (valleys[level][i].l == w) {
                if (i > 0)
                    valleys[level+1][i-1].r += w;
                valleys[level+1][i].l = valleys[level][i].l;
                valleys[level+1][i].r = valleys[level][i].r - w;
                valleys[level+1][i].y = valleys[level][i].y;
                valleys[level+1][i].x = valleys[level][i].x + w;
                valleys[level+1][i].done = 0;
                for (int j = i+1; j < width; j++)
                    valleys[level+1][j] = valleys[level][j];
                logs[level][0] = i+1;
                logs[level][1] = w;
                searchT(level+1, width);
            } else {
                valleys[level+1][i].l = valleys[level][i].l - w;
                valleys[level+1][i].r = valleys[level][i].r;
                valleys[level+1][i].y = valleys[level][i].y + w;
                valleys[level+1][i].x = valleys[level][i].x;
                valleys[level+1][i].done = 0;
                valleys[level+1][i+1].l = valleys[level][i+1].l + w;
                valleys[level+1][i+1].r = valleys[level][i+1].r;
                valleys[level+1][i+1].y = valleys[level][i+1].y;
                valleys[level+1][i+1].x = valleys[level][i+1].x;
                valleys[level+1][i+1].done = valleys[level][i+1].done;
                for (int j = i+2; j < width; j++)
                    valleys[level+1][j] = valleys[level][j];
                logs[level][0] = i+1;
                logs[level][1] = w;
                searchT(level+1, width);
            }
        }
    }
}

double b;

static void search(int level, int width, int bound) {

    if (width == 0) {
        throw found(level);
    }

    if (width > bound) {
        return;
    }

    if (width == bound) {
        start = clock();
        searchT(level, width);
        stop = clock();
        all += (stop - start);
        return;
    }
    if (width == bound-1) {
        searchT1(level, width);
        return;
    }

    for (int i = 0; i < width; i++) {
        byte w = std::min(valleys[level][i].l, valleys[level][i].r);
        byte w1 = valleys[level][i].done + 1;
        if (w1 <= w) {
            for (int j = 0; j < i; j++) {
                valleys[level + 1][j] = valleys[level][j];
                valleys[level + 1][j].done =
                        std::min(valleys[level][j].l, valleys[level][j].r);
            }
            if (w1 < w) {
                valleys[level + 1][i].l = valleys[level][i].l - w1;
                valleys[level + 1][i].r = w1;
                valleys[level + 1][i].y = valleys[level][i].y + w1;
                valleys[level + 1][i].x = valleys[level][i].x;
                valleys[level + 1][i].done = 0;
                valleys[level + 1][i + 1].l = w1;
                valleys[level + 1][i + 1].r = valleys[level][i].r - w1;
                valleys[level + 1][i + 1].y = valleys[level][i].y;
                valleys[level + 1][i + 1].x = valleys[level][i].x + w1;
                valleys[level + 1][i + 1].done = 0;
                for (int j = i + 1; j < width; j++)
                    valleys[level + 1][j + 1] = valleys[level][j];
                logs[level][0] = i + 1;
                for (;;) {
                    logs[level][1] = w1;
                    search(level + 1, width + 1, bound - 1);
                    if(++w1 >= w)
                        break;
                    valleys[level + 1][i].l--;
                    valleys[level + 1][i].r = w1;
                    valleys[level + 1][i].y++;
                    valleys[level + 1][i].x = valleys[level][i].x;
                    valleys[level + 1][i + 1].l = w1;
                    valleys[level + 1][i + 1].r--;
                    valleys[level + 1][i + 1].y = valleys[level][i].y;
                    valleys[level + 1][i + 1].x++;
                }
            }
            if (valleys[level][i].l == valleys[level][i].r && level != 0) {
                if (i > 0)
                    valleys[level + 1][i - 1].r += w;
                valleys[level + 1][i].l = valleys[level][i + 1].l + w;
                valleys[level + 1][i].r = valleys[level][i + 1].r;
                valleys[level + 1][i].y = valleys[level][i + 1].y;
                valleys[level + 1][i].x = valleys[level][i + 1].x;
                valleys[level + 1][i].done = valleys[level][i + 1].done;
                for (int j = i + 1; j < width - 1; j++)
                    valleys[level + 1][j] = valleys[level][j + 1];
                logs[level][0] = i + 1;
                logs[level][1] = w;
                search(level + 1, width - 1, bound - 1);
            } else if (valleys[level][i].l == w) {
                if (i > 0)
                    valleys[level + 1][i - 1].r += w;
                valleys[level + 1][i].l = valleys[level][i].l;
                valleys[level + 1][i].r = valleys[level][i].r - w;
                valleys[level + 1][i].y = valleys[level][i].y;
                valleys[level + 1][i].x = valleys[level][i].x + w;
                valleys[level + 1][i].done = 0;
                for (int j = i + 1; j < width; j++)
                    valleys[level + 1][j] = valleys[level][j];
                logs[level][0] = i + 1;
                logs[level][1] = w;
                search(level + 1, width, bound - 1);
            } else {
                valleys[level + 1][i].l = valleys[level][i].l - w;
                valleys[level + 1][i].r = valleys[level][i].r;
                valleys[level + 1][i].y = valleys[level][i].y + w;
                valleys[level + 1][i].x = valleys[level][i].x;
                valleys[level + 1][i].done = 0;
                valleys[level + 1][i + 1].l = valleys[level][i + 1].l + w;
                valleys[level + 1][i + 1].r = valleys[level][i + 1].r;
                valleys[level + 1][i + 1].y = valleys[level][i + 1].y;
                valleys[level + 1][i + 1].x = valleys[level][i + 1].x;
                valleys[level + 1][i + 1].done = valleys[level][i + 1].done;
                for (int j = i + 2; j < width; j++)
                    valleys[level + 1][j] = valleys[level][j];
                logs[level][0] = i + 1;
                logs[level][1] = w;
                search(level + 1, width, bound - 1);
            }
        }
    }
}

void decryptCoords(int bound, std::stringstream &s) {

    for (int lvl = 0; lvl < bound; lvl++) {
        int order = logs[lvl][0] - 1;
        s << int(valleys[lvl][order].x) << " " << int(valleys[lvl][order].y) << " " << int(logs[lvl][1]) << std::endl;
    }

}

int main(int argc, char **argv) {

    int x, y;

    std::cin >> x;

    b = 6*log2(x);

    /*if(x == 29) {
        std::cout << "14\n"
                     "0 0 5\n"
                     "0 5 4\n"
                     "4 5 1\n"
                     "5 0 6\n"
                     "4 6 3\n"
                     "0 9 7\n"
                     "7 6 4\n"
                     "11 0 5\n"
                     "11 5 5\n"
                     "7 10 6\n"
                     "0 16 13\n"
                     "13 10 3\n"
                     "16 0 13\n"
                     "13 13 16" << std::endl;
        return 0;
    } else if(x == 31) {
        std::cout << "15\n"
                     "0 0 1\n"
                     "0 1 1\n"
                     "1 0 2\n"
                     "0 2 3\n"
                     "3 0 5\n"
                     "0 5 8\n"
                     "8 0 13\n"
                     "0 13 18\n"
                     "21 0 10\n"
                     "21 10 1\n"
                     "22 10 1\n"
                     "21 11 2\n"
                     "18 13 5\n"
                     "23 10 8\n"
                     "18 18 13" << std::endl;
        return 0;
    } else if(x == 37) {
        std::cout << "15\n"
                     "0 0 8\n"
                     "0 8 5\n"
                     "5 8 3\n"
                     "5 11 1\n"
                     "5 12 1\n"
                     "0 13 6\n"
                     "8 0 11\n"
                     "6 11 8\n"
                     "14 11 5\n"
                     "14 16 3\n"
                     "17 16 2\n"
                     "17 18 1\n"
                     "0 19 18\n"
                     "19 0 18\n"
                     "18 18 19" << std::endl;
        return 0;
    }*/

    y = x;
    std::string reason, coords;
    int bound = int(b);
    valleys[0][0].l = x;
    valleys[0][0].r = y;
    valleys[0][0].x = 0;
    valleys[0][0].y = 0;
    valleys[0][0].done = 0;
    for (;;) {
        try {
            search(0, 1, bound - 1);
        } catch (found f) {
            bound = f.level;
            continue;
        }
        std::stringstream s;
        s << bound << std::endl;
        decryptCoords(bound, s);
        reason = s.str();
        break;
    }

    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;

    printf("\nTime elapsed: %.5f\n", double(all) / CLOCKS_PER_SEC);

    std::cout << reason << std::endl;
}