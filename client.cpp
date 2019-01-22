#include <string>
#include <stdio.h>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <netdb.h> 
#include <ncurses.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void getdir(int sockfd, WINDOW *dir)
{
    int direction_new;
    int direction;
    while(true)
    {
        direction_new = wgetch(dir);
        if (direction_new == 113) {
            int converted_number;
            converted_number = htonl(0);
            write(sockfd, &converted_number, sizeof(converted_number));
            close(sockfd);
            break;
        }
        else if(direction_new > 0 && direction != direction_new)
        {
            int converted_number;
            converted_number = htonl(direction_new);
            write(sockfd, &converted_number, sizeof(converted_number));
            direction = direction_new;
        }
    }
}


int main(int argc, char *argv[])
{
    int sockfd, portno;
    char name[10];
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 2) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    initscr();
    WINDOW *win = newwin(22, 71, 2, 5);
    WINDOW *scor = newwin(22, 30, 2, 5 + 71);
    WINDOW *dir = newwin(1, 1, LINES-1, COLS-1);

    wprintw(win, "Please enter your name:");
    wgetstr(win, name);
    wclear(win);
    write(sockfd, name, 10);


    raw();
    noecho();
    touchwin(win);

    std::thread t1(getdir, sockfd, dir); 
    t1.detach();

    int n;
    struct dataSend
    {
        char snake_dis[22][70];
        char playerName[10][10];
        int scoreList[10];
    }a;
    memset(&a, 0, sizeof(a));
    while(true)
    {
        n = read(sockfd,&a,sizeof(a));
        if (n < 0 ) {
            close(sockfd);
            endwin();
            system("clear");
            break;
        }

        for(int i=0;i<22;i++)
        {
            for(int j=0;j<70;j++)
            {
                wprintw(win, "%c", a.snake_dis[i][j]);
            }
            wprintw(win, "\n");
        } 
        mvwprintw(scor, 1, 1, "Player");
        mvwprintw(scor, 1, 20, "Score");
        for (int i = 0; i < 10; i++) {
            mvwprintw(scor, i+3, 1, "%s", a.playerName[i]);
            mvwprintw(scor, i+3, 22, "%d", a.scoreList[i]);
        }
        box(win, 0, 0);
        box(scor, 0, 0);
        wrefresh(win);
        wrefresh(scor);
        wclear(win);
    }
    close(sockfd);
    endwin();
    system("clear");
    return 0;
}
