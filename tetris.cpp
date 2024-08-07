#include <iostream>
#include <boost\asio.hpp>
#include <thread>
#include <conio.h>
#include <string.h>


class Screen{
    public:
    Screen(boost::asio::io_context & io): _strand(boost::asio::make_strand(io)),_UpdateTimer(io,boost::asio::chrono::seconds(1)),
    _SpawnTimer(io,boost::asio::chrono::seconds(1))
    {
        for(int i = 0;i < 16;i++)
            for(int j = 0;j < 32;j++)
                map[i][j] = 0;
        _UpdateTimer.async_wait(boost::asio::bind_executor(_strand,std::bind(&Screen::update,this)));
        _SpawnTimer.async_wait(boost::asio::bind_executor(_strand,std::bind(&Screen::spawn,this)));
    }
    void update()
    {
        // Logic update
        if(kbhit()){
            int key = getch();
            if(key == 77) {
                for(int i = 0;i < 16; i++){
             for(int j = 31;j > 0;j--)
                if(map[i][j-1]==1 && map[i][j]==0) {
                    map[i][j-1] = 0;
                    map[i][j] = 1;
                }
            }
            c++;
        }
        if(key == 75){
                for(int i = 0;i < 16; i++){
                    for(int j = 0;j < 31;j++)
                    if(map[i][j+1]==1 && map[i][j]==0) {
                        map[i][j+1] = 0;
                        map[i][j] = 1;
                    }
                }
            c--;
        }
            std::cout << key << " " << c << std::endl;
        }
        for(int i = 15;i > 0; i--){
             for(int j = 0;j < 32;j++){
                if(map[i-1][j]==1 && map[i][j]==0) {
                    map[i-1][j] = 0;
                    map[i][j] = 1;
                }
                else if(map[i][j]==2){
                    map[i-1][j] = 2;
                }
            }
        }

        // Screen update
        //system("cls");
        std::string _t = "";
        for(int i = 0;i < 16;i++){
            for(int j = 0;j < 32;j++){
                if(map[i][j]>0) _t += "#";
                else _t += "*";
            }
            _t += "\n";
        }
        std::cout << _t << c << std::endl;
        
        _UpdateTimer.expires_at(_UpdateTimer.expiry() + boost::asio::chrono::seconds(1));
        _UpdateTimer.async_wait(std::bind(&Screen::update,this));
    }

    void spawn() {
        map[0][15] = 1;
        map[0][16] = 1;
        map[0][17] = 1;
        map[0][18] = 1;

        //_SpawnTimer.expires_at(_SpawnTimer.expiry() + boost::asio::chrono::seconds(2));
        //_SpawnTimer.async_wait(std::bind(&Screen::spawn,this));
        
    }

    boost::asio::strand<boost::asio::io_context::executor_type> getStrand(){
        return _strand;
    }

    private:
        int map[16][32];
        boost::asio::strand<boost::asio::io_context::executor_type> _strand;
        boost::asio::steady_timer _UpdateTimer;
        boost::asio::steady_timer _SpawnTimer;

        int c = 0;
};

int main() {
    boost::asio::io_context io;
    Screen sc(io);

    std::thread th([&]{io.run();});
    th.join();

    return 0;
}