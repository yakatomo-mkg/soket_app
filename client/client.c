#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main() {
    const int port_number = 12345;
    // 1. ソケットファイルディスクリプタ(=「コネクトソケット」）の作成 : socket()
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
    // ソケットのセットアップ(クライアント側では、"接続先の設定(server変数に格納)")
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // 接続先のIPアドレス(ループバックアドレス(=自分自身)を設定)
    server.sin_port = htons(port_number);

    // 2. サーバーへ接続
    if((connect(sock, (struct sockaddr *)&server, sizeof(server))) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }
    // 3. サーバーとの送受信
    // サーバーへ送信
    char* hello = "Hello from Client";
    send(sock, hello, strlen(hello), 0);
    printf("Message sent to server\n");
    // サーバーから受信
    char buff[1024] = {0};
    recv(sock, buff, sizeof(buff), 0);
    printf("received message is \"%s\"\n", buff);
    // 4. ソケットを閉じる（接続を切断）
    close(sock);
    return 0;
}