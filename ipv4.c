#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct sockaddr_in taAddress;
typedef struct Msg
{
    int complete;
    char buf[500];
} msg;
msg* received;
char buffer[500];
void initTAaddr(char *argv[]);
void readFromTA(int sock);
void writeID(int sock);
void writeIPv6(int sock);
void sendPort(int sock);
void writeY(int sock);
void writeOk(int sock);
void writeAnswer(int sock);
int main(int argc, char *argv[])
{

    if (argc == 3)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            printf("error while creating client socket!");
            return 0;
        }
        initTAaddr(argv);
        int conn = connect(sock, (struct sockaddr *)&taAddress, sizeof(taAddress));
        if (conn == -1)
        {
            printf("error while connecting the server!");
            return 0;
        }
        for (int i = 0; i < 6; i++)
        {
            readFromTA(sock);
            switch (i)
            {
            case 0:
                writeID(sock);
                break;
            case 1:
                writeIPv6(sock);
                break;
            case 2:
                sendPort(sock);
                break;
            case 3:
                writeY(sock);
                break;
            case 4:
                writeY(sock);
                break;
            case 5:
                writeOk(sock);
                break;
            default:
                break;
            }
        }
        // ftok to generate unique key
        key_t key = ftok("resfile", 75);

        // shmget returns an identifier in shmid
        int shmid = shmget(key, sizeof(msg), 0666 | IPC_CREAT);

        received = shmat(shmid, NULL, 0);
        while (received->complete == 0)
        {
            printf("still processing on ipv6\n");
            sleep(3);
        }
        printf("ipv6 sends  results successfully\n");
        printf("Received from ipv6: %s\n", received->buf);
        writeAnswer(sock);
        readFromTA(sock);
        //destroy the shared memory
        shmctl(shmid, IPC_RMID, NULL);
        return 0;
    }
    printf("input has errors\n");
    return 0;
}

void initTAaddr(char *argv[])
{
    memset(&taAddress, 0x00, sizeof(taAddress));
    taAddress.sin_family = AF_INET;
    taAddress.sin_addr.s_addr = inet_addr(argv[1]);
    taAddress.sin_port = htons(atoi(argv[2]));
}

void readFromTA(int sock)
{
    memset(buffer, 0x00, sizeof(buffer));
    read(sock, buffer, sizeof(buffer));
    printf("%s", buffer);

    memset(buffer, 0x00, sizeof(buffer));
    read(sock, buffer, sizeof(buffer));
    printf("%s", buffer);
}

void writeID(int sock)
{
    memset(buffer, 0x00, sizeof(buffer));
    read(0, buffer, sizeof(buffer));
    write(sock, buffer, strlen(buffer));
}

void writeIPv6(int sock)
{
    memset(buffer, 0x00, sizeof(buffer));
    read(0, buffer, sizeof(buffer));
    write(sock, buffer, strlen(buffer));
}

void sendPort(int sock)
{
    memset(buffer, 0x00, sizeof(buffer));
    read(0, buffer, sizeof(buffer));
    write(sock, buffer, strlen(buffer));
}

void writeY(int sock)
{
    memset(buffer, 0x00, sizeof(buffer));
    read(0, buffer, sizeof(buffer));
    write(sock, buffer, strlen(buffer));
}

void writeOk(int sock)
{
    memset(buffer, 0x00, sizeof(buffer));
    read(0, buffer, sizeof(buffer));
    write(sock, buffer, strlen(buffer));
}

void writeAnswer(int sock){
    
    memset(buffer, 0x00 , sizeof(buffer));
    memcpy(buffer, received->buf, strlen(received->buf));
    write(sock, buffer, strlen(buffer));
}
