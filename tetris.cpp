#include <iostream>
#include <conio.h>
#include <string>
#include <windows.h>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <ctime>

int speed = 10; //*24 ms
bool isLife = true;
bool _wasSwaped = false;
std::mutex locker;
int actualShape, _x_, _y_;
int savedShape = -1;
int nextShape;
int rotationState = 1;
int map[16][16];
int shape[7][4][4][2] ={{{{0,0},{0,-1},{0,2},{0,1}},{{0,0},{1,0},{-2,0},{-1,0}},{{0,0},{0,-1},{0,-2},{0,1}},{{0,0},{1,0},{2,0},{-1,0}}},
                    {{{0,0},{1,0},{-1,0},{1,1}},{{0,0},{0,-1},{0,1},{-1,1}},{{0,0},{1,0},{-1,0},{-1,-1}},{{0,0},{0,-1},{0,1},{1,-1}}},
                    {{{0,0},{1,0},{-1,0},{-1,1}},{{0,0},{0,-1},{0,1},{-1,-1}},{{0,0},{1,0},{-1,0},{1,-1}},{{0,0},{0,-1},{0,1},{1,1}}},
                    {{{0,0},{1,0},{-1,0},{0,1}},{{0,0},{0,1},{0,-1},{-1,0}},{{0,0},{1,0},{-1,0},{0,-1}},{{0,0},{0,1},{0,-1},{1,0}}},
                    {{{-1,0},{-2,0},{-1,1},{0,1}},{{-1,0},{-1,1},{0,0},{0,-1}},{{-1,0},{-2,0},{-1,1},{0,1}},{{-1,0},{-1,1},{0,0},{0,-1}}},
                    {{{0,0},{1,0},{0,1},{-1,1}},{{0,0},{0,1},{-1,0},{-1,-1}},{{0,0},{1,0},{0,1},{-1,1}},{{0,0},{0,1},{-1,0},{-1,-1}}},
                    {{{0,0},{-1,0},{-1,1},{0,1}},{{0,0},{-1,0},{-1,1},{0,1}},{{0,0},{-1,0},{-1,1},{0,1}},{{0,0},{-1,0},{-1,1},{0,1}}}};
int BlockPos[2][4];  // {{xcords},{ycords}}
int lines = 0;

class Screen{
    public:
    Screen()
    {
        for(int i = 0;i < sizeof(map)/sizeof(map[0]);i++)
            for(int j = 0;j < sizeof(map[0])/sizeof(0);j++)
                map[i][j] = 0;
    }
    void update()
    {
        // Logic update
        locker.lock();
        bool _isGrounded = false;
        for (int i = 0; i < 4; i++){
            //std::cout << BlockPos[0][i] << " : " << BlockPos[1][i] << " -> " << map[BlockPos[1][i]][BlockPos[0][i]] << std::endl;
            if((map[BlockPos[1][i]+1][BlockPos[0][i]] < 0 || BlockPos[1][i] == sizeof(map)/sizeof(map[0]) - 1) && map[BlockPos[1][i]][BlockPos[0][i]] == 1) 
                        _isGrounded = true;
        }
        bool _isFalling = false;
        for (int i = 0; i < 4; i++){
            if(_isGrounded){
                map[BlockPos[1][i]][BlockPos[0][i]] = -1;
            }
            else if(map[BlockPos[1][i]][BlockPos[0][i]] > 1) {
                    map[BlockPos[1][i]][BlockPos[0][i]]--;
            }
            else{
                map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                BlockPos[1][i]++;
                _isFalling = true;
            }
        }
        if (_isFalling)
        {
            _y_++;
            for (int i = 0; i < 4; i++)
            map[BlockPos[1][i]][BlockPos[0][i]] = speed;
        }
        
        if(_isGrounded)
        {
            _wasSwaped = false;
            this->spawn(rand() % 7,false);
        }

        for(int i = 0;i < sizeof(map)/sizeof(map[0]);i++){
            bool _isFull = true;
            for(int j = 0;j < sizeof(map[0])/sizeof(0);j++){
                if(map[i][j] >= 0) _isFull = false;
            }
            if(_isFull){
                if(speed > 1) speed--;
                lines += 1;
                for(int k = i;k >= 0; k--){
                    for(int j = 0;j < sizeof(map[0])/sizeof(0);j++){
                        if(map[k-1][j] <= 0 && k != 0)map[k][j] = map[k-1][j];
                        else map[k][j] = 0;
                    }
                }
            }
        }
        // Screen update
        system("cls");
        std::string _t = "";
        for(int i = 0;i < 3;i++){
            if(i == 1){
                for(int j = 0;j < (sizeof(map[0])/4 - 13) / 2;j++){
                    _t+="@";
                }
                _t += " - TETRIS BY TIGMEN - ";
                for(int j = 0;j < (sizeof(map[0])/4 - 13) / 2 + 1;j++){
                    _t+="@";
                }
                _t+= "\n";
            }
            else{
            for(int j = 0;j < sizeof(map[0])/4 + 9;j++){
                    _t+="@";
                }
                _t+="\n";
            }
        }
        for(int i = 0;i < (sizeof(map)/sizeof(map[0]));i++){
            _t += "@";
            for(int j = 0;j < sizeof(map[0])/4;j++){
                if(map[i][j] > 0) _t += "0";
                else if(map[i][j] < 0) _t += "#";
                else _t += ".";
            }
            for(int j = 0;j < 8;j++){
                    if(i == sizeof(map[0])/4 - 1){
                        _t+="@";
                    }
                    else if(i > sizeof(map[0])/4 - 5){
                        bool _ = false;
                        if(nextShape != -1)
                        for(int k = 0;k < 4;k++)
                            if(shape[nextShape][1][k][1] == i + 3 - sizeof(map[0])/4 && shape[nextShape][1][k][0] == j - 4){
                                _t +="0";
                                _ = true;
                            }
                        if(!_){
                        if(j == 0 || j == 7) _t+="@";
                        else _t+=".";
                        }
                    }
                    else if(i == sizeof(map[0])/4 - 5){
                        _t += "@-NEXT-@";
                        break;
                    }
                    else if(i == sizeof(map[0])/4 - 6 || i == sizeof(map[0])/4 - 7){
                        _t+="@";
                    }
                    else if(i > sizeof(map[0])/4 - 11){
                        bool _ = false;
                        if(savedShape != -1)
                        for(int k = 0;k < 4;k++)
                            if(shape[savedShape][1][k][1] == i + 9 - sizeof(map[0])/4 && shape[savedShape][1][k][0] == j - 4){
                                _t +="0";
                                _ = true;
                            }
                        if(!_){
                        if(j == 0 || j == 7) _t+="@";
                        else _t+=".";
                        }
                    }
                    else if(i == sizeof(map[0])/4 - 11){
                        _t += "@-SAVE-@";
                        break;
                    }
                    else if(i == 1){
                        if(j == 0) _t+="@";
                        else if(j < 6-lines/10) _t+="0";
                        else{
                            _t+=std::to_string(lines) + "@";
                            break;
                        } 

                    }
                    else if(i == 0){
                        _t += "@LEVELS@";
                        break;
                    }
                    else
                    _t+="@";
                }
                _t+="\n";
        }
        for(int j = 0;j < sizeof(map[0])/4 + 9;j++){
                    _t+="@";
                }
        locker.unlock();
        std::cout << _t << std::endl;


        Sleep(24);
    
    }
        

    void spawn(int param,bool swapped) {
        if(!swapped) actualShape = nextShape;
        else actualShape = param;
        rotationState = 1;
        _x_ = 8;
        _y_ = 1;
        for(int i = 0;i < 4;i++){
            map[_y_ + shape[actualShape][rotationState][i][1]][_x_ + shape[actualShape][rotationState][i][0]] = speed;
            BlockPos[0][i] = _x_ + shape[actualShape][rotationState][i][0];
            BlockPos[1][i] = _y_ + shape[actualShape][rotationState][i][1];
        }
        if(!swapped) nextShape = param;
    }
    
};
class Controller{
    public:
    Controller(Screen sc){
        _sc = sc;
    }
    void control() {
        if(kbhit()){
            int key = getch();
            locker.lock();

            if(key == 75) {
                bool _isBlocked = false;
                for (int i = 0; i < 4; i++)
                    if(map[BlockPos[1][i]][BlockPos[0][i] - 1] < 0 || BlockPos[0][i] == 0) _isBlocked = true;
                if(!_isBlocked){
                    int _localSpeed = map[BlockPos[1][0]][BlockPos[0][0]];
                    for (int i = 0; i < 4; i++){
                        map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                        BlockPos[0][i]--;
                    }
                    _x_--;
                    for (int i = 0; i < 4; i++) map[BlockPos[1][i]][BlockPos[0][i]] = _localSpeed;
                }
                
            }

            if(key == 77){
                bool _isBlocked = false;
                for (int i = 0; i < 4; i++)
                    if(map[BlockPos[1][i]][BlockPos[0][i] + 1] < 0 || BlockPos[0][i] == sizeof(map[0])/4 - 1) _isBlocked = true;
                if(!_isBlocked){
                    int _localSpeed = map[BlockPos[1][0]][BlockPos[0][0]];
                    for (int i = 0; i < 4; i++){
                        map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                        BlockPos[0][i]++;
                    }
                    _x_++;
                    for (int i = 0; i < 4; i++) map[BlockPos[1][i]][BlockPos[0][i]] = _localSpeed;
                }
            }

            if(key == 80){
                bool _isGrounded = false;
                for (int i = 0; i < 4; i++){
                    if(map[BlockPos[1][i]+1][BlockPos[0][i]] < 0 || BlockPos[1][i] == sizeof(map)/sizeof(map[0]) - 1) _isGrounded = true;
                }
                if(_isGrounded)
                {
                    for (int i = 0; i < 4; i++) 
                        map[BlockPos[1][i]][BlockPos[0][i]] = -1;
                    _sc.spawn(rand() % 7,false);
                    _wasSwaped = false;
                }
                else{
                    for (int i = 0; i < 4; i++){
                        map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                        BlockPos[1][i]++;
                    }
                    _y_++;
                    for (int i = 0; i < 4; i++)
                    map[BlockPos[1][i]][BlockPos[0][i]] = speed;
                }
            }
            
            if(key == 72){
                bool _isBlocked = false;
                int _localSpeed = map[BlockPos[1][0]][BlockPos[0][0]];
                //std::cout << _x_ << " : " << _y_ << std::endl;
                
                for(int i = 0;i < 4;i++){
                    int _ = map[_y_ + shape[actualShape][rotationState+1][i][1]][_x_ + shape[actualShape][rotationState+1][i][0]];
                    if(_ == -1 || _y_ + shape[actualShape][rotationState+1][i][1] < 0 || _y_ + shape[actualShape][rotationState+1][i][1] >= sizeof(map)/sizeof(map[0]) 
                    || _x_ + shape[actualShape][rotationState+1][i][0] < 0 || _x_ + shape[actualShape][rotationState+1][i][0] >= sizeof(map[0])/4) _isBlocked = true;
                }
                if(!_isBlocked){
                    if(rotationState < 3) rotationState++;
                    else rotationState = 0;
                    for(int i = 0;i < 4;i++) map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                    for(int i = 0;i < 4;i++){
                        map[_y_ + shape[actualShape][rotationState][i][1]][_x_ + shape[actualShape][rotationState][i][0]] = _localSpeed;
                        BlockPos[0][i] = _x_ + shape[actualShape][rotationState][i][0];
                        BlockPos[1][i] = _y_ +shape[actualShape][rotationState][i][1];
                        //std::cout << BlockPos[0][i] << " : " << BlockPos[1][i] << " -> " << map[BlockPos[1][i]][BlockPos[0][i]] << " x " <<map[_y_ + shape[actualShape][rotationState][i][1]][_x_ + shape[actualShape][rotationState][i][0]] << std::endl;
                    }
                }
            }

            if(key == 32){
                if(!_wasSwaped){
                if(savedShape < 0){
                    savedShape = actualShape;
                    for (int i = 0; i < 4; i++) map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                    _sc.spawn(rand() % 7,false);
                }
                else{
                    int _ = savedShape;
                    for (int i = 0; i < 4; i++) map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                    savedShape = actualShape;
                    _sc.spawn(_,true);
                }
                _wasSwaped = true;
                }
            }

            if(key == 27) isLife = false;
            locker.unlock();
        }
    }
    private:
    Screen _sc;
};

int main() {
    system("color 02");
    
    Screen sc = Screen();
    Controller ct = Controller(sc);

    srand(time(0));
    nextShape = rand() % 7;
    sc.spawn(4,false);

    while(isLife){
    std::thread scr([&]{
        srand(time(0));
        sc.update();});
    scr.join();
    ct.control();
    }
    system("color 07");
    return 0;
}