#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    const int port_number = 12345;

    // 1. create socket（クライアント側のソケットを作成）
    int client_fd = 0;
    if ((client_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error");
        exit(EXIT_FAILURE);
    }
    printf("Client_socket_fd_number : %d\n", client_fd);  // クライアント側のソケットディスクリプタを確認

    // 接続先のサーバー情報を指定
    struct sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port_number);

    // 2. connect to server
    if ((connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0) {
        printf("\nConnect failed\n");
        exit(EXIT_FAILURE);
    }

    // 3. read or write data with server
    // 3-1. send data to server
    const int MAX_LENGTH = 100; // 入力最大文字列の長さは100とする
    char str[MAX_LENGTH + 1];  // +1 は終端のnull文字('\0')分として確保

    printf("Enter your favorite number: ");
    // scanf()は、空白文字(改行, スペース, タブ)を除く文字列を受け取る
    // ※ 例えば、"Hello World" と入力された場合、「Hello」だけを受け取る
    scanf("%100s", str);  // フォーマット文字列で、文字列の幅を100に指定
    send(client_fd, str, strlen(str), 0);  // サーバーにデータを送信
    // printf("Message sent to server\n");

    // 3-2. read data from server
    char buff[1024] = {0};
    recv(client_fd, buff, sizeof(buff), 0);
    printf("Your favorite number is \"%s\"\n", buff);

    // socket close
    close(client_fd);
    return 0;
}