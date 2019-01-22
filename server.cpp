#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <vector>
#include <curses.h>
#include <queue>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ncurses.h>

using namespace std;

struct dataSend
{
    char snake_dis[22][70];
    char playerName[10][10];
    int scorelist[10];
}a;

const int mAX_sise=3;  
mutex mtx; // 线程锁

void getdir(int, int&, bool&); // 获取方向
void error(const char *msg) // 错误处理
{
    perror(msg);
    exit(1);
}

/*************************游戏模块********************************************************************/
class Cmp
{
    
    friend class Csnake;
    int rsign;
    int lsign;
    public:
    Cmp(int r,int l){setpoint(r,l);}
    Cmp(){}
    void setpoint(int r,int l)
    {
        rsign=r;
        lsign=l;
    }
    Cmp operator-(const Cmp &m)const
    {
        return Cmp(rsign-m.rsign,lsign-m.lsign);
    }
    Cmp operator+(const Cmp &m)const
    {
        return Cmp(rsign+m.rsign,lsign+m.lsign);
    }
};

class Csnake
{
    Cmp firstsign;
    Cmp secondsign;
    Cmp nextsign;
    Cmp lastsign;

    int count;//snake lenth
    public:
    void show();
    char name[10];
    bool ifdead;
    ~Csnake(){

        for(int i=0;i<3+score/20;i++)
        {
            lastsign=snakebody.front();
            a.snake_dis[lastsign.lsign][lastsign.rsign]='*';
            snakebody.pop();
        }
    }
    queue<Cmp>snakebody;
    char getsymbol(const Cmp &c)const//get specic point address
    {
        return a.snake_dis[c.lsign][c. rsign];
    }
    bool isdead(const Cmp &cmp)
    {
        return(getsymbol(cmp)=='@'||cmp.rsign==70||cmp.rsign==0||cmp.lsign==21||cmp.lsign==0);
    }
    int score=0;
    void init();//fist display
    void update(int&);
};
void Csnake::init()
{
    int k;
    int t;
    ifdead = true;
    srand(time(0));
    t=(rand()%(18))+2;
    k=(rand()%(50))+8;
    if(a.snake_dis[t][k]!='@'&&a.snake_dis[t][k]!='*'&&a.snake_dis[t][k+1]!='@'&&a.snake_dis[t][k+1]!='*'&&a.snake_dis[t][k+2]!='@'&&a.snake_dis[t][k+2]!='*')

    {
        for(int m=k;m<mAX_sise+k;m++)
        {
            a.snake_dis[t][m]='@';
            snakebody.push(Cmp(m,(t)));
        }
        firstsign=snakebody.back();
        secondsign.setpoint(mAX_sise+k-2,t);
    }
    else{cout << "init loading is falled" << endl;}
}



void Csnake::update(int &converted_number)
{
    int r,l;
   static int n=0;
        do
        {
            srand(time(0));
            r=(rand()%(67))+1;
            l=(rand()%(20))+1;
            if(a.snake_dis[l][r]!='@')
            {
           
                if(n%5==0)
                { a.snake_dis[l][r]='#';}
                else if(n%3==0)
                {
                a.snake_dis[l][r]=' ';
                }
                else
                {
                    a.snake_dis[l][r]='*';
                }
            }
            
            n=n+1;
        }while(a.snake_dis[l][r]=='@'&& n%1==0);
    switch(converted_number)//init nextsign
    {
        case 'e':{if(firstsign.rsign!=secondsign.rsign)
                     nextsign.setpoint(firstsign.rsign,firstsign.lsign-1);
                     else nextsign=firstsign+(firstsign-secondsign);}break;//up_key
        case 'd':{if(firstsign.rsign!=secondsign.rsign)nextsign.setpoint(firstsign.rsign,firstsign.lsign+1);
                     else nextsign=firstsign+(firstsign-secondsign);}break;//down_key
        case 's':{if(firstsign.lsign!=secondsign.lsign)nextsign.setpoint(firstsign.rsign-1,firstsign.lsign);
                     else nextsign=firstsign+(firstsign-secondsign);}break;//rignt_key
        case 'f':{if(firstsign.lsign!=secondsign.lsign)nextsign.setpoint(firstsign.rsign+1,firstsign.lsign);
                     else nextsign=firstsign+(firstsign-secondsign);}break;//left_key
        default:nextsign=firstsign+(firstsign-secondsign);

    }
    if(getsymbol(nextsign)!='*'&&getsymbol(nextsign)!='#'&&!isdead(nextsign))//no eat and no dead
    {
        lastsign=snakebody.front();
        a.snake_dis[lastsign.lsign][lastsign.rsign]=' ';
        snakebody.pop();
        secondsign=firstsign;
        snakebody.push(nextsign);
        firstsign=snakebody.back();
        a.snake_dis[firstsign.lsign][firstsign.rsign]='@';
        ifdead = true;
    }
    else if(getsymbol(nextsign)=='*'&&!isdead(nextsign))//eat food no dead
    {
        secondsign=firstsign;
        snakebody.push(nextsign);
        firstsign=snakebody.back();
        a.snake_dis[firstsign.lsign][firstsign.rsign]='@';
        score+=20;
        ifdead = true;

    }
    else if(getsymbol(nextsign)=='#'&&!isdead(nextsign))//eat bed food no dead
     {
         lastsign=snakebody.front();
         a.snake_dis[lastsign.lsign][lastsign.rsign]=' ';
         snakebody.pop();
         lastsign=snakebody.front();
          a.snake_dis[lastsign.lsign][lastsign.rsign]=' ';
          snakebody.pop();
         secondsign=firstsign;
         snakebody.push(nextsign);
         firstsign=snakebody.back();
         a.snake_dis[firstsign.lsign][firstsign.rsign]='@';
         score-=20;
         if(score==-40)
         {  ifdead = false;}
         else
         {
         ifdead = true;
         }
     }
    else
    {
        ifdead = false;
    }

}

void Csnake::show()
{
    for(int i=0;i<22;i++)
    {
        for(int j=0;j<70;j++)
        {cout<<a.snake_dis[i][j];
        }
        cout<<endl;
    }

}


/*************************控制模块********************************************************************/

void getdir(int newsockfd, int &converted_number, bool &ifdead)
{
    while(ifdead)
    {
        int dir; // 方向的阿斯克码
        int n; // socket连接状态
        int tmp; // 同步状态
        n = read(newsockfd, &dir, sizeof(dir));
        if (n < 0) {
            cout << "ERROR on reading." << endl;
        }

        tmp = ntohl(dir); // 网络格式数字转为计算机型
        if (tmp > 0) {
            converted_number = tmp; // 排除其他非法方向指令
        }
        if (tmp == 0) {
            ifdead = false;
            break;
        }
    }
    close(newsockfd);
}

void start(int newsockfd, int threadNumber, int &sync)
{
    int converted_number;  // 方向指令
    int tmp = sync;  // 同步状态
    int n;
    Csnake s;

    n = read(newsockfd, s.name, 10); // 接受玩家名
    if (n < 0) {
        cout << "ERROR on reading." << endl;
    }
    cout << s.name << " join the game." << endl;
    mtx.lock();  // 上锁，避免多线程同时对玩家列表写入
    strcpy(a.playerName[threadNumber], s.name);  // 将当前玩家名加入玩家列表
    mtx.unlock(); // 写入完成，解锁
    s.init();  // 玩家的初始化
    thread t1(getdir, newsockfd, ref(converted_number), ref(s.ifdead)); // 接受玩家方向指令
    t1.detach(); 

    do{
        if (tmp != sync) { // 同步状态判断 
            mtx.lock();
            s.update(converted_number);  // 蛇移动
            a.scorelist[threadNumber] = s.score; // 将当前玩家的分写入得分列表
            mtx.unlock();
            n = write(newsockfd,&a,sizeof(a)); // 发送游戏状态
            if (n < 0) {
                cout << "ERROR on writing." << endl;
            }
            tmp = sync; // 更新同步判断
        }
    }while(s.ifdead);
    close(newsockfd);
    cout << s.name << " leave the game." << endl;
}

void syncfun(int &sync) // 同步控制
{
    while(true)
    {
        sync += 1; // 全局时钟
        this_thread::sleep_for(chrono::milliseconds(300)); // 时钟频率1/300ms
    }
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    memset(&a, 0, sizeof(a));
    for(int i=0;i<22;i++) // 初始化地图
    {
        for(int j=0;j<70;j++)
        {a.snake_dis[i][j]=' ';}
    }
    if (argc < 2) { // 没有输入端口号
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); // string型数字转为int型
    serv_addr.sin_family = AF_INET; // IPV4族
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(portno); 
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5); // 最多同时接受5个socket
    clilen = sizeof(cli_addr);

    int sync = 0;  // 全局时钟
    thread t1(syncfun, ref(sync));
    t1.detach();

    int threadNumber = 0;  // 线程编号

    while (1) {
        newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        thread t(start, newsockfd, threadNumber, ref(sync));
        t.detach();
        threadNumber += 1;
        }
    close(sockfd);
    return 0; /* we never get here */
}


