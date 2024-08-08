#include <iostream>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <ctime>

int speed = 20; //*24 ms
bool isLife = true;
std::mutex locker;
int map[8][16];
int shape[][4][2] ={{{0,0},{1,0},{2,0},{-1,0}},
                    {{0,0},{1,0},{-1,0},{1,1}},
                    {{0,0},{1,0},{-1,0},{-1,1}},
                    {{0,0},{1,0},{-1,0},{0,1}},
                    {{0,0},{-1,0},{0,1},{1,1}},
                    {{0,0},{1,0},{0,1},{-1,1}},
                    {{0,0},{1,0},{1,1},{0,1}}};
int BlockPos[2][4];  // {{xcords},{ycords}}

class Screen{
    public:
    Screen()
    {
        for(int i = 0;i < 16;i++)
            for(int j = 0;j < 32;j++)
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
            for (int i = 0; i < 4; i++)
            map[BlockPos[1][i]][BlockPos[0][i]] = speed;
        }
        
        if(_isGrounded) this->spawn(rand() % 7);
        // Screen update
        system("cls");

        std::string _t = "@@@@@@@@@@@@@@@@@@\n@@@ - TETRIS - @@@\n@@@@@@@@@@@@@@@@@@\n";
        for(int i = 0;i < sizeof(map)/sizeof(map[0]);i++){
            _t += "@";
            for(int j = 0;j < sizeof(map[0])/sizeof(0);j++){
                if(map[i][j] > 0) _t += "0";
                else if(map[i][j] < 0) _t += "#";
                else _t += ".";
            }
            _t += "@\n";
        }
        _t += "@@@@@@@@@@@@@@@@@@\n";
        locker.unlock();
        std::cout << _t << std::endl;
        Sleep(24);
    
    }
        

    void spawn(int param) {
        for(int i = 0;i < 4;i++){
            map[shape[param][i][1]][8+shape[param][i][0]] = speed;
            BlockPos[0][i] = 8+shape[param][i][0];
            BlockPos[1][i] = shape[param][i][1];
        }
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
                    if(map[BlockPos[1][i]][BlockPos[0][i] - 1] < 0) _isBlocked = true;
                if(!_isBlocked){
                    int _localSpeed = map[BlockPos[1][0]][BlockPos[0][0]];
                    for (int i = 0; i < 4; i++){
                        map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                        BlockPos[0][i]--;
                    }
                    for (int i = 0; i < 4; i++) map[BlockPos[1][i]][BlockPos[0][i]] = _localSpeed;
                }
                
            }
            
            if(key == 77){
                bool _isBlocked = false;
                for (int i = 0; i < 4; i++)
                    if(map[BlockPos[1][i]][BlockPos[0][i] + 1] < 0) _isBlocked = true;
                if(!_isBlocked){
                    int _localSpeed = map[BlockPos[1][0]][BlockPos[0][0]];
                    for (int i = 0; i < 4; i++){
                        map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                        BlockPos[0][i]++;
                    }
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
                    _sc.spawn(rand() % 7);
                }
                else{
                    for (int i = 0; i < 4; i++){
                        map[BlockPos[1][i]][BlockPos[0][i]] = 0;
                        BlockPos[1][i]++;
                    }
                    for (int i = 0; i < 4; i++)
                    map[BlockPos[1][i]][BlockPos[0][i]] = speed;
                }
            }
            
            if(key == 72){} //rotate
            if(key == 32){} //save
            if(key == 27) isLife = false;
            locker.unlock();
        }
    }
    private:
    Screen _sc;
};

int main() {
    system("color 05");
    
    Screen sc = Screen();
    Controller ct = Controller(sc);

    sc.spawn(rand() % 7);


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