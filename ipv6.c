#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PORT_NUM 55555
#define QUEUE_LENGTH 3
#define TOKEN_REQ 5
#define MAX_CHILD 3
#define IP_ADDRESS "2001:0:c38c:c38c:1c4f:2c88:5082:3c5c"

pid_t waitPid;
socklen_t addressLen;
struct sockaddr_in6 taAddress, serverAddress;
typedef struct Msg
{
    int complete;
    char buf[500];
} msg;
char result[500];
char result1[50];
char result2[50];
char result3[50];
char result4[50];
char result5[50];
char dataBuffer[250];
char convertAddress[INET6_ADDRSTRLEN];
void initArray();
void initServer();
void savingValue();
void settingValue();
int main(void)
{
    int fileDescriptor[2];
    int listen_sock = -1, client_sock = -1, sock;
    initArray();
    listen_sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == -1)
    {
        return 0;
    }
    printf("socket created\n");
    initServer();
    sock = bind(listen_sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (sock == -1)
    {
        printf("bind error\n");
        close(listen_sock);
        return 0;
    }
    sock = listen(listen_sock, QUEUE_LENGTH);
    if (sock == -1)
    {
        printf("listen error\n");
        close(listen_sock);
        return 0;
    }
    addressLen = sizeof(taAddress);
    int received = 0, childCnt = 0;
    while (received < TOKEN_REQ)
    {
        printf("wait for new connection \n");
        client_sock = accept(listen_sock, (struct sockaddr *)&taAddress, &addressLen);
        inet_ntop(AF_INET6, &(taAddress.sin6_addr), convertAddress, sizeof(convertAddress));
        printf("new connection from %s\n", convertAddress);
        pipe(fileDescriptor);
        int status;
        int pid = fork();
        if (pid > 0)
        {
            childCnt++;
            if (childCnt >= MAX_CHILD)
            {
                while (true)
                {
                    waitPid = wait(&status);
                    if (waitPid == -1)
                    {
                        printf("wait error\n");
                        continue;
                    }
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {
                        childCnt--;
                        printf("child count decrease :%d\n", childCnt);
                        break;
                    }
                }
            }
            read(fileDescriptor[0], dataBuffer, sizeof(dataBuffer));
            received++;
            savingValue();
            if (received == 5) settingValue(received);
            close(client_sock);
        }
        else if (pid == 0)
        {
            while (true)
            {
                memset(dataBuffer, 0x00, sizeof(dataBuffer));
                int len = read(client_sock, dataBuffer, 30);
                if (len <= 0)
                {
                    close(client_sock);
                    break;
                }
                printf("reading tokens from ta client %d\n", received + 1);
                printf("buffer : ");
                for (int i = 0; i < strlen(dataBuffer) - 1; i++)
                {
                    printf("%c", dataBuffer[i]);
                }
                write(fileDescriptor[1], dataBuffer, sizeof(dataBuffer));
                printf("socket close client %d\n\n", received + 1);
            }
            exit(0);
        }
        else
        {
            perror("fork fail\n");
            return -1;
        }
        printf("Received: %s\n", result);
    }

    // ipv4 send part
    key_t key = ftok("resfile", 75);

    // shmget returns an identifier in shmid
    int shmid = shmget(key, sizeof(msg), 0666 | IPC_CREAT);

    // shmat to attach to shared memory
    msg *res = shmat(shmid, NULL, 0);
    char *bufPtr;
    bufPtr = res->buf;
    res->complete = 0;
    memcpy(res->buf, result, strlen(result));
    printf("writing process , shared Memeory Write : Wrote %lu bytes\n", strlen(res->buf));
    res->complete = 1;
    shmdt(res);
    return 0;
}

void initArray()
{
    memset(result, 0x00, sizeof((result)));
    memset(dataBuffer, 0x00, sizeof(dataBuffer));
}

void initServer()
{
    serverAddress.sin6_family = AF_INET6;
    char convertAddress[16];
    inet_pton(AF_INET6, IP_ADDRESS, (void *)&convertAddress);
    memcpy((void *)&serverAddress.sin6_addr.s6_addr, (void *)convertAddress, 16);
    serverAddress.sin6_port = htons(PORT_NUM);
}

void savingValue()
{
    if (dataBuffer[6] == '1')
    {
        strcat(result1, dataBuffer);
        result1[strlen(result1) - 2] = ',';
        result1[strlen(result1) - 1] = '\0';
    }
    else if (dataBuffer[6] == '2')
    {
        strcat(result2, dataBuffer);
        result2[strlen(result2) - 2] = ',';
        result2[strlen(result2) - 1] = '\0';
    }
    else if (dataBuffer[6] == '3')
    {
        strcat(result3, dataBuffer);
        result3[strlen(result3) - 2] = ',';
        result3[strlen(result3) - 1] = '\0';
    }
    else if (dataBuffer[6] == '4')
    {
        strcat(result4, dataBuffer);
        result4[strlen(result4) - 2] = ',';
        result4[strlen(result4) - 1] = '\0';
    }
    else if (dataBuffer[6] == '5')
    {
        strcat(result5, dataBuffer);
        result5[strlen(result5) - 2] = 0x0a;
    }
    else
        return;
}

void settingValue(int received)
{
    strcat(result, result1);
    strcat(result, result2);
    strcat(result, result3);
    strcat(result, result4);
    strcat(result, result5);
}
