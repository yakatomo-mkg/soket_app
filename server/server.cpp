#include <iostream>
#include <sys/socket.h>  // アドレスドメイン
#include <sys/types.h>  // ソケットタイプ
#include <arpa/inet.h>  // バイトオーダーの変換に利用
#include <unistd.h>  // close()に利用
#include <string>
#include <cstring>

int main() {
    const int port_number = 12345;
    // 1. ソケットファイルディスクリプタ(=「リッスンソケット」）の作成 : socket()
    // 引数: アドレスドメイン, ソケットタイプ(int型), プロトコル(int型)
    int sock_fd = 0;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // エラー処理
        std::cerr << "Error socket : " << std::strerror(errno);
        exit(EXIT_FAILURE);  // 異常終了
    }

    // ソケットのセットアップ(サーバー側では、"新しい接続を待ち受けるための設定"を行う)
    // ソケット通信において接続情報を保持するための構造体変数「server」を用意
    struct sockaddr_in server;
    // memset(&server, 0, sizeof(sockaddr_in));  // memsetで初期化
    server.sin_family = AF_INET;  // IPv4アドレスファミリ
    server.sin_addr.s_addr = htonl(INADDR_ANY);  // INADDR_ANY:「どのクライアントからの接続も受け入れる」定数
    server.sin_port = (in_port_t)htons(port_number);  // ポート番号

    // 2. ソケットにIPアドレスとポート番号を割り当てる（=OSに伝える）: bind()
    // 引数: 作成したソケット, アドレスポインタ, アドレスサイズ
    if (bind(sock_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        // エラー処理
        std::cerr << "Error bind : " << std::strerror(errno);
        exit(EXIT_FAILURE);
    }

    // 3. クライアントからの接続要求を待ち受ける : listen()
    // 引数: 作成したソケット、同時に待ち受けるリクエスト数(上限はOSによる)
    // SOMAXCONN : 受け付けることができる接続要求のキューの最大長を示す定数
    if(listen(sock_fd, 3) < 0) {
        // エラー処理
        std::cerr << "Error listen : " << std::strerror(errno);
        exit(EXIT_FAILURE);
    } else {
        std::cout << "接続を待機しています..." << std::endl;
    }

    /* 4. クライアントからの接続要求を一つずつ受け入れる
          (クライアントからの接続を受け入れる毎に新たなソケットを作成する) */ 
    // 引数: 最初に作成した代表受付用ソケット(int), 接続相手のソケットアドレスポインタ、接続相手のアドレスサイズ
    struct sockaddr_in client; // 接続相手のソケットアドレス
    socklen_t client_len = sizeof(struct sockaddr_in); // 接続相手のアドレスサイズ
    int sock = 0;
    if ((sock = accept(sock_fd, (struct sockaddr *)&client, &client_len)) < 0) {
        // エラー処理
        std::cerr << "Error accept : " << std::strerror(errno);
        exit(EXIT_FAILURE);
    }

    // 5. クライアントとの送受信
    // クライアントからのデータを受信
    char buff[1024] = {0};  // 1024バイトのデータを格納するための配列を初期化
    recv(sock, buff, sizeof(buff), 0);  // 受信
    // 「\"」はダブルクォーテーションのエスケープシーケンス
    std::cout << "received message is \"" << buff << "\"" << std::endl;

    // クライアントにデータを送信
    const char *hello = "Hello from server";
    send(sock, hello, strlen(hello), 0);
    std::cout << "Message sent to client" << std::endl;

    // 6. クライアントとの通信を終了（切断）
    close(sock);    // close connecting soket(特定のクライアントとの接続を切断)
    shutdown(sock_fd, SHUT_RDWR);  // リスニングソケット(おおもとのソケット)の通信をシャットダウンする
    close(sock_fd); // close listening socket(リスニングソケットを閉じる)

    return 0;
}
