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
        
        std::vector<std::vector<int>> _temp;
        bool _isGrounded = false;
         for(int i = sizeof(map)/sizeof(map[0]) - 1; i >= 0; i--)
             for(int j = 0;j < sizeof(map[0])/4;j++){
                if(map[i][j] > 0){
                    std::vector<int> _{i,j};
                    _temp.push_back(_);
                    if((map[i+1][j] < 0 || i == sizeof(map)/sizeof(map[0]) - 1) && map[i][j] == 1) 
                        _isGrounded = true;
                } 
            }
        
        for (int i = 0; i < _temp.size(); i++){
            if(_isGrounded){
                map[_temp[i][0]][_temp[i][1]] = -1;
            }
            else if(map[_temp[i][0]][_temp[i][1]] > 1) {
                    map[_temp[i][0]][_temp[i][1]]--;
            }
            else{
                map[_temp[i][0]][_temp[i][1]] = 0;
                map[_temp[i][0]+1][_temp[i][1]] = speed;
            }
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
        }
    }
    
};
class Controller{
    public:
    void control() {
        if(kbhit()){
            int key = getch();
            locker.lock();
            if(key == 75) {
                
                for(int i = 0;i < sizeof(map)/sizeof(map[0]); i++){
                    for(int j = 0;j < sizeof(map[0])/4 - 1;j++)
                        if(map[i][j+1]>0 && map[i][j]==0) {
                            map[i][j] = map[i][j+1];
                            map[i][j+1] = 0;
                        }
                }
            }
            if(key == 77){
                for(int i = 0;i < sizeof(map)/sizeof(map[0]); i++){
                    for(int j = sizeof(map[0])/4 - 1;j > 0;j--)
                        if(map[i][j-1]>0 && map[i][j]==0) {
                            map[i][j] = map[i][j-1];
                            map[i][j-1] = 0;
                        }
                }   
                
            }
            locker.unlock();
        }
    }
};

int main() {
    system("color 05");

    Screen sc = Screen();
    Controller ct;

    sc.spawn(rand() % 7);

    while(isLife){
    std::thread scr([&]{
        srand(time(0));
        sc.update();});
    scr.join();
    ct.control();
    }

    return 0;
}