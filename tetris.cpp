#include <iostream>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <thread>
#include <mutex>

int speed = 30;
bool isLife = true;
std::mutex locker;
int map[8][16];

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
        for(int i = 7;i >= 0; i--){
             for(int j = 0;j < 16;j++){
                if(map[i-1][j]==1 && map[i][j]==0 && i > 0) {
                    std::cout << i << " " << j << std::endl;
                    map[i][j] = speed;
                    map[i-1][j] = 0;
                }
                if(map[i][j] == -1 && map[i-1][j] == 1){
                    map[i-1][j] = -1;
                    this->spawn();
                }
                else if(map[i][j] == 1 && i == 7){
                    map[i][j] = -1;
                    this->spawn();
                }
                else if(map[i][j]>1){
                    map[i][j]--;
                } 
            }
        }

        // Screen update
        system("cls");
        std::string _t = "";
        for(int i = 0;i < 8;i++){
            for(int j = 0;j < 16;j++){
                if(map[i][j]!=0) _t += "#";
                else _t += ".";
            }
            _t += "\n";
        }
        locker.unlock();
        std::cout << _t << std::endl;
        Sleep(10);
        
    }

    void spawn() {
        map[0][2] = speed; 
        map[0][3] = speed;
        map[0][4] = speed;
        map[0][5] = speed;
    }
};
class Controller{
    public:
    void control() {
        if(kbhit()){
            int key = getch();
            locker.lock();
            if(key == 75) {
                
                for(int i = 0;i < 8; i++){
                    for(int j = 0;j < 15;j++)
                        if(map[i][j+1]>0 && map[i][j]==0) {
                            map[i][j] = map[i][j+1];
                            map[i][j+1] = 0;
                        }
                }
            }
            if(key == 77){
                for(int i = 0;i < 8; i++){
                    for(int j = 15;j > 0;j--)
                        if(map[i][j-1]>0 && map[i][j]==0) {
                            map[i][j] = map[i][j-1];
                            map[i][j-1] = 0;
                        }
                }   
                
            }
            locker.unlock();
        }
    }

    private: 

};

int main() {
    system("color 05");
    Screen sc = Screen();
    Controller ct;
    sc.spawn();
    while(isLife){
    std::thread scr([&]{sc.update();});
    scr.join();
    ct.control();
    }
    return 0;
}