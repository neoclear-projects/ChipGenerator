#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <Fl/fl_ask.H>
#include <Fl/Fl_Input.H>
#include <Fl/Fl_Bitmap.H>
#include <Fl/Fl_RGB_Image.H>
#include <Fl/Fl_BMP_Image.H>
#include <bits/stdc++.h>
#include <libpng16/png.h>

using namespace std;

vector<int> DIRS = {-1, 0, 1, 0, -1};

int gen_rand(int l, int r) {
    return (rand() % (r - l)) + l;
}

class MapData {
public:
    unsigned char *meta;
    int x, y;
    MapData(int x_, int y_): x(x_), y(y_) {
        meta = (unsigned char*)malloc(sizeof(unsigned char) * x_ * y_ * 3);
    }
    unsigned char *getData() {
        return meta;
    }
    void setPixel(int x_, int y_, vector<int> color) {
        uint64_t offset = (x_ * y + y_) * 3;
        *(meta + offset) = color[0];
        *(meta + offset + 1) = color[1];
        *(meta + offset + 2) = color[2];
    }
    void setRectangle(pair<int, int> s, pair<int, int> e, vector<int> color) {
        for (int i = s.first; i < e.first; i++) {
            for (int j = s.second; j < e.second; j++)
                setPixel(i, j, color);
        }
    }
};

uint64_t tag(int x, int y) {
    return ((uint64_t)x << 32) | (uint64_t)y;
}

vector<vector<int>> gen(int x, int y) {
    vector<vector<int>> g = vector<vector<int>>(x, vector<int>(y, INT_MIN));
    queue<vector<int>> q;
    unordered_set<uint64_t> visited;
    int high_point_num = gen_rand(2, 5);
    for (int i = 0; i < high_point_num; i++)
        q.push({gen_rand(0, x), gen_rand(0, y), gen_rand(400, 900), gen_rand(-20, 0)});
    while (!q.empty()) {
        int size = q.size();
        while (size--) {
            auto cur = q.front();
            q.pop();
            if (visited.count(tag(cur[0], cur[1])))
                continue;
            visited.insert(tag(cur[0], cur[1]));
            g[cur[0]][cur[1]] = cur[2];
            int trend = cur[3] + gen_rand(-10 - cur[3] / 4, 10 -cur[3] / 4 + 1);
            for (int i = 0; i < 4; i++) {
                int tx = cur[0] + DIRS[i];
                int ty = cur[1] + DIRS[i + 1];
                if (tx < 0 || ty < 0 ||
                    tx >= x || ty >= y ||
                    visited.count(tag(tx, ty)))
                    continue;
                
                int height = cur[2] + trend + gen_rand(-10, 11);
                int tr = trend + gen_rand(-2, 3);
                q.push({tx, ty, height, tr});
            }
        }
    }
    return g;
}

void quit_application(Fl_Widget *w, void *v) {
    exit(0);
}

void save_file(Fl_Widget *w, void *v) {
}

void change_image(Fl_Widget *w, void *v) {
    MapData *dat = new MapData(600, 600);
    auto heights = gen(600, 600);
    for (int i = 0; i < 600; i++) {
        for (int j = 0; j < 600; j++) {
            auto cur = heights[i][j];
            if (cur > 600)
                dat->setPixel(i, j, {160, 82, 45});
            else if (cur > 300)
                dat->setPixel(i, j, {0, 250 - cur / 10, 0});
            else if (cur > 0)
                dat->setPixel(i, j, {50, 205, 50});
            else
                dat->setPixel(i, j, {30, 144, 255});
        }
    }
    Fl_RGB_Image *bit = new Fl_RGB_Image(dat->getData(), 600, 600);
    ((Fl_Box *)v)->image(bit);
    ((Fl_Box *)v)->redraw();
}

int main() {
    srand((unsigned)time(NULL));
    Fl_Window *window = new Fl_Window(1200, 900);
    window->begin();
    Fl_Box *box = new Fl_Box(100, 100, 600, 600, "");

    Fl_Button *change = new Fl_Button(800, 100, 150, 100, "Change"),
              *save   = new Fl_Button(800, 300, 150, 100, "Save"),
              *quit   = new Fl_Button(800, 500, 150, 100, "Quit");
              
    change->labelsize(24);
    save->labelsize(24);
    quit->labelsize(24);

    change->callback(change_image, box);
    save->callback(save_file, box);
    quit->callback(quit_application);
    window->end();
    Fl::visual(FL_RGB);
    window->show();
    return Fl::run();
}
