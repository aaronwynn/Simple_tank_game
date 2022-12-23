#include <iostream>
#include <vector>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std;

template<class T>
vector<T> operator + (vector<T> v, vector<T> w){
    if (v.size() != w.size()){
        throw invalid_argument("not same length");
    }
    for (int i=0; i<v.size(); i++){
        v[i] += w[i];
    }
    return v;
}

template<class T>
vector<T> operator * (double a, vector<T> v){
    for (int i=0; i<v.size(); i++){
        v[i] *= a;
    }
    return v;
}

template<class T>
ostream& operator << (ostream& stream, vector<T> v){
    stream << "{";
    for (int i=0; i<v.size()-1; i++){
        stream << v[i] << ", ";
    }
    stream << v[v.size()-1] << "}";
    return stream;
}

class Tank{
public:
    vector<vector<short>> physics;  // [[height array], [width array]], the first eight entries are where the body is
    vector<vector<short>>* collision;   // collision stays constant
    short len_phy;
    vector<vector<char>>* frame;
    short angle;    // can be from {0, ..., 4}
    short h;
    short w;
    short screen_width;
    short power;    // can be from {0, ..., 4}
    bool direct;
    Tank(vector<vector<char>>* f, vector<vector<short>>* coll, bool dir, short start_height, short start_width, short screen_w):
    frame(f),
    h(start_height),
    w(start_width),
    collision(coll),
    screen_width(screen_w),
    angle(0),
    power(0),
    physics(2, vector<short>(11)),
    len_phy(11),
    direct(dir)
    {
        plot();
        int index=0;
        for (short i = -2; i < 3; i++){
            physics[0][index] = h;
            physics[1][index] = w+i;
            index++;
        }
        for (short i = -1; i < 2; i++){
            physics[0][index] = h+1;
            physics[1][index] = w+i;
            index++;
        }
        if (direct) {
            physics[0][index] = h + 1;
            physics[1][index++] = w - 2;
            physics[0][index] = h + 1;
            physics[1][index++] = w - 3;
            physics[0][index] = h + 1;
            physics[1][index++] = w - 4;
        } else {
            physics[0][index] = h + 1;
            physics[1][index++] = w + 2;
            physics[0][index] = h + 1;
            physics[1][index++] = w + 3;
            physics[0][index] = h + 1;
            physics[1][index++] = w + 4;
        }
    }
    bool left(){    // returns if going left comes through
        for (int i=0; i<len_phy; i++){
            physics[1][i] -= 1;
        }
        for (int i=0; i<len_phy; i++){
            for (int j=0; j<(*collision)[0].size(); j++){
                if (check_border(i, j)){
                    // there is a collision
                    for (i=0; i<len_phy; i++){
                        physics[1][i] += 1;
                    }
                    return false;
                }
            }
        }
        w--;
        return true;
    }
    bool check_border(int i, int j){    // check if i-th phy in j-th collision
        if ((physics[0][i] == (*collision)[0][j] && physics[1][i] == (*collision)[1][j]) || physics[1][i] <= 0 || physics[1][i] > screen_width){
            return true;
        }
        return false;
    }
    bool right(){   // returns if going right comes through
        for (int i=0; i<len_phy; i++){
            physics[1][i] += 1;
        }
        for (int i=0; i<len_phy; i++){
            for (int j=0; j<(*collision)[0].size(); j++){
                if (check_border(i, j)){
                    // there is a collision
                    for (i=0; i<len_phy; i++){
                        physics[1][i] -= 1;
                    }
                    return false;
                }
            }
        }
        w++;
        return true;
    }
    bool up(){  // returns if up came through   TODO: haven't programmed real collision with barrel yet
        if (angle != 4){
            angle++;
            if (direct==0){
                if (angle == 1) {
                    physics[0][10]++;
                    physics[1][10]++;
                }
                if (angle == 2) {
                    physics[0][9]++;
                    physics[1][9]++;
                }
                if (angle == 3) {
                    len_phy--;
                    physics[0][8]++;
                    physics[1][9]--;
                }
                if (angle == 4) {
                    len_phy--;
                    physics[1][8]--;
                }
            } else {
                if (angle == 1) {
                    physics[0][10]++;
                    physics[1][10]--;
                }
                if (angle == 2) {
                    physics[0][9]++;
                    physics[1][9]--;
                }
                if (angle == 3) {
                    len_phy--;
                    physics[0][8]++;
                    physics[1][9]++;
                }
                if (angle == 4) {
                    len_phy--;
                    physics[1][8]++;
                }
            }
        }
        return false;
    }
    bool down(){    // return if down came through
        if (angle != 0){
            angle--;
            if (direct==0) {
                if (angle == 0) {
                    physics[0][10]--;
                    physics[1][10]--;
                }
                if (angle == 1) {
                    physics[0][9]--;
                    physics[1][9]--;
                }
                if (angle == 2) {
                    len_phy++;
                    physics[0][8]--;
                    physics[1][9]++;
                    physics[0][10] = h + 2;
                    physics[1][10] = w + 5;
                }
                if (angle == 3) {
                    len_phy++;
                    physics[1][8]++;
                    physics[0][9] = h + 2;
                    physics[1][9] = w + 3;
                }
            } else {
                if (angle == 0) {
                    physics[0][10]--;
                    physics[1][10]++;
                }
                if (angle == 1) {
                    physics[0][9]--;
                    physics[1][9]++;
                }
                if (angle == 2) {
                    len_phy++;
                    physics[0][8]--;
                    physics[1][9]--;
                    physics[0][10] = h + 2;
                    physics[1][10] = w - 5;
                }
                if (angle == 3) {
                    len_phy++;
                    physics[1][8]--;
                    physics[0][9] = h + 2;
                    physics[1][9] = w - 3;
                }
            }
        }
        return false;
    }



    void plot(){
        (*frame)[h][w] = 'O';
        (*frame)[h][w+1] = '_';
        (*frame)[h][w+2] = 'O';
        (*frame)[h][w-1] = '_';
        (*frame)[h][w-2] = 'O';
        (*frame)[h+1][w] = '#';
        (*frame)[h+1][w+1] = '#';
        (*frame)[h+1][w-1] = '#';
        short v;
        if (direct) {
            v = -1;
        }
        else { v = 1; }
        if (angle == 0){
            (*frame)[h+1][w+v*2] = '-';
            (*frame)[h+1][w+v*3] = '-';
            (*frame)[h+1][w+v*4] = '-';
        }
        if (angle == 1){
            (*frame)[h+1][w+v*2] = '-';
            (*frame)[h+1][w+v*3] = '"';
            (*frame)[h+2][w+v*5] = '-';
        }
        if (angle == 2){
            (*frame)[h+1][w+v*2] = '"';
            (*frame)[h+2][w+v*4] = '-';
            (*frame)[h+2][w+v*5] = '"';
        }
        if (angle == 3){
            (*frame)[h+2][w+v*2] = '-';
            (*frame)[h+2][w+v*3] = '"';
        }
        if (angle == 4){
            (*frame)[h+2][w+v*1] = '|';
        }

//         ###---
//        O_O_O

//               _
//         ###-"
//        O_O_O
//
//              _"
//         ###"
//        O_O_O
//
//            _"
//         ###
//        O_O_O
//
//           |
//         ###
//        O_O_O
    }
    void del(){
        for (int i=0; i<len_phy; i++){
            (*frame)[physics[0][i]][physics[1][i]] = ' ';
        }
    }
};









class Shot{
public:
    vector<double> v;   // velocity [height, width]
    vector<double> pos; // position [height, width]
    double g;   // gravity
    double c;   // air resistance
    vector<vector<char>>* frame;
    vector<vector<short>>* collision;
    vector<vector<short>>* tank0;   // physiscs of tank0
    vector<vector<short>>* tank1;   // physiscs of tank1
    short screen_width;
    short screen_height;
    bool direction;
    Shot(short angle, short power, vector<vector<char>>* f, vector<vector<short>>* coll, vector<vector<short>>* t1,
    vector<vector<short>>* t2, double start_height, double start_width, short screen_w, short screen_h, bool dir):
    g(0.25),
    c(0.95),
    frame(f),
    collision(coll),
    tank0(t1),
    tank1(t2),
    pos(2),
    screen_width(screen_w),
    screen_height(screen_h),
    direction(dir),
    v(2)
    {
        // cout << endl << "SHOOT0";
        pos[0] = start_height;
        pos[1] = start_width;
        //cout << endl << "SHOOT1";
        int s = 1;
        if (direction){
            s = -1;
        }
        if (angle==0){
            pos[0] = start_height;
            pos[1] = start_width + s*1;
            v[0] = 0;
            v[1] = 1;
        }
        if (angle==1){
            pos[0] = start_height;
            pos[1] = start_width + s*1;
            v[0] = 0.315;
            v[1] = 0.95;
        }
        if (angle==2){
            pos[0] = start_height+1;
            pos[1] = start_width+s*1;
            v[0] =  0.5    ;// 0.7071067811;
            v[1] =  0.86602540378    ;// 0.7071067811;
        }
        if (angle==3){
            pos[0] = start_height+1;
            pos[1] = start_width+s*1;
            v[0] = 0.75;
            v[1] = 0.66143;
        }
        if (angle==4){
            pos[0] = start_height+1;
            pos[1] = start_width+s*1;
            v[0] = 0.9;
            v[1] = 0.4359;
        }

        // cout << endl << "SHOOT2";
        v = (double)(++power) * v;
        if (direction) {
            v[1] *= -1;
        }
        // cout << endl << "SHOOT" << v;
    }
    void draw(){
        (*frame)[round(pos[0])][round(pos[1])] = '*';
    }
    short step(){   // returns 0 if tank0 is hit, 1 if tank1 is hit, 2 if mountain  is hit or out of screen, 3 if still flying
        v[0] -= g;
        v[1] *= c;
        //cout << "Pos: " << pos;
        // cout << "Vel: " << v;
        (*frame)[round(pos[0])][round(pos[1])] = ' ';
        // cout << "STEP0" << endl << pos << endl;
        short erg = check_all();
        pos = pos + v;
        // cout << "STEP1" << endl << pos << "erg:" << erg << endl;
        return erg;
    }
    short check_all(){
        if (check_collision_specific(tank0)){
            return 0;
        }
        if (check_collision_specific(tank1)){
            return 1;
        }
        if (check_collision_specific(collision)){
            return 2;
        }
        if (pos[0] + v[0] <0 || pos[0] + v[0]>screen_height-1 || pos[1] + v[1]<0 || pos[1] + v[1]>screen_width-1){// check if outside screen
            return 2;
        }
        return 3;
    }
    bool check_collision_specific(vector<vector<short>>* coll){
        for (double i=0; i<1; i += 0.05){
            for (int j=0; j<(*coll)[0].size(); j++){
                // cout << "Pos:" << pos+(i*v);
                // cout << i*v << i;
                if (in(pos+(i*v),   (*coll)[0][j], (*coll)[1][j])){
                    // cout << (*coll)[0][j] << (*coll)[1][j];
                    return true;
                }
            }
        }
        return false;
    }
    bool in(vector<double> p, short height, short width){
        if (p[0]>=height && p[0]<height+0.999 && p[1]>=width && p[1]<width+0.999){
            return true;
        }
        return false;
    }
};
class Engine{
public:
    short width;
    short height;
    vector<vector<char>> frame;
    vector<vector<short>> collision;
    Tank t0;
    Tank t1;
    vector<Tank*> tanks;
    bool turn;
    Engine(short w=90, short h=15):
    collision(2, vector<short> (20)),
    width(w),
    height(h),
    frame(height, vector<char>(width, ' ')),
    t0(&frame, &collision, false, 0, 15, width),
    t1(&frame, &collision, true, 0, 60, width),
    turn(false)
    {
        tanks = {&t0, &t1};
        create_mountain();
    }
    short shoot(short angle, short power){
        cout << endl << "func shoot";
        vector<vector<short>> tank0(2, vector<short>(tanks[0]->len_phy));
        vector<vector<short>> tank1(2, vector<short>(tanks[1]->len_phy));
        for (int i=0; i<tanks[0]->len_phy; i++){
            tank0[0][i] = tanks[0]->physics[0][i];
            tank0[1][i] = tanks[0]->physics[1][i];
        }
        for (int i=0; i<tanks[1]->len_phy; i++){
            tank1[0][i] = tanks[1]->physics[0][i];
            tank1[1][i] = tanks[1]->physics[1][i];
        }
        cout << endl << "init";
        Shot s(angle, power, &frame, &collision, &tank0, &tank1, tanks[turn]->physics[0][tanks[turn]->len_phy-1], tanks[turn]->physics[1][tanks[turn]->len_phy-1], width, height, turn);
        short erg;
        int index = 0;
        s.draw();
        plot_frame();
        erg = s.step();
        while (erg == 3){
            cout << "loop" << index++ << endl;
            s.draw();
            plot_frame();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            erg = s.step();
        }
        return erg;
    }
    void plot_frame(){
        for (int i=0; i<20; i++){
            cout << endl;
        }
        for (short i=height-1; i>0; i--){
            cout << "|";
            for (short j=0; j<width; j++){
                cout << frame[i][j];
            }
            cout << "|" << endl;
        }
        cout << "|";
        for (int j=0; j<width; j++){
            cout << frame[0][j];
        }
        cout << "| Player " << turn + 1 << endl;
        for (int i=0; i<width+2; i++){
            cout << "_";
        }
        cout << " angle: " << (*tanks[turn]).angle + 1 << " Power: " << (*tanks[turn]).power + 1;
    }
    void create_mountain(){
        short index = 0;
        for (short i=40; i<51; i++){
            frame[0][i] = '#';
            collision[0][index] = 0;
            collision[1][index++] = i;
        }
        for (short i=42; i<49; i++){
            frame[1][i] = '#';
            collision[0][index] = 1;
            collision[1][index++] = i;
        }
        frame[2][45] = '#';
        collision[0][index] = 2;
        collision[1][index++] = 45;
        frame[2][46] = '#';
        collision[0][index] = 2;
        collision[1][index] = 46;
    }
    void run(){
        plot_frame();
        bool no_one_dead = true;
        while (!(GetKeyState('Q') & 0x8000) && no_one_dead){
            if (GetKeyState('A') & 0x8000){ // move left
                (*tanks[turn]).del();
                (*tanks[turn]).left();
                (*tanks[turn]).plot();
                plot_frame();
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
            if (GetKeyState('D') & 0x8000){ // move right
                (*tanks[turn]).del();
                (*tanks[turn]).right();
                (*tanks[turn]).plot();
                plot_frame();
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
            if (GetKeyState('W') & 0x8000){ // angle up
                (*tanks[turn]).del();
                (*tanks[turn]).up();
                (*tanks[turn]).plot();
                plot_frame();
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
            if (GetKeyState('S') & 0x8000){ // angle down
                (*tanks[turn]).del();
                (*tanks[turn]).down();
                (*tanks[turn]).plot();
                plot_frame();
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
            if (GetKeyState('X') & 0x8000){ // shoot
                short erg;
                erg = shoot((*tanks[turn]).angle, (*tanks[turn]).power);
                if (erg == 0){
                    no_one_dead = false;
                    cout << endl << endl << endl << "THE WINNER IS PLAYER 2";
                    goto a;
                }
                if (erg == 1){
                    no_one_dead = false;
                    cout << endl << endl << endl << "THE WINNER IS PLAYER 1";
                    goto a;
                }
                turn = !turn;
                (*tanks[turn]).plot();
                plot_frame();
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                a:;
            }
            if (GetKeyState('P') & 0x8000){ // more power
                if ((*tanks[turn]).power != 6){
                    (*tanks[turn]).power++;
                    plot_frame();
                } else {
                    plot_frame();
                    cout << " Max power reached";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
            if (GetKeyState('L') & 0x8000){ // less power
                if ((*tanks[turn]).power != 0){
                    (*tanks[turn]).power--;
                    plot_frame();
                } else {
                    plot_frame();
                    cout << " Min power reached";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
        }
    }
};

int main(){
    Engine e;
    e.run();

    return 0;
}