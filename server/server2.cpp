// 入力した数値を素数判定するプログラム
#include <iostream>
#include <unistd.h>
#include <string>
#include <cmath>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// 与えられた文字列を`str`で受け取り、`unsigned long 型`(数値)に変換 & valに格納して「true」or「false」を返す関数
bool StrToValue(unsigned long &val, const std::string &str) {
    try {
        val = std::stoul(str); // stoul()で文字列を `unsigned long`型に変換
        return true;
    }
    // 例外をキャッチ(「(...)」は "全ての例外(全ての型の例外)")
    // 例えば、文字列が数値でない、数値が大きすぎる、負の数の場合、stoul()は例外を投げるためcatchブロックで受け取る
    catch(...) {
        return false;
    }
}


// 与えられた数値`val`が素数であるかどうかを判定する関数
bool IsPrime(const unsigned long &val) {
    if (val < 2)  // 0または1の場合
        return false;
    if (val == 2)
        return false;
    if (val % 2 == 0)  // 偶数の場合
        return false;
    
    // valが1以外の奇数の場合
    // 「2」と「valの平方根」の間の奇数を使って、valの約数を検出する
    unsigned long sq_val = static_cast<unsigned long>(std::sqrt(val));
    for (unsigned long i = 3; i <= sq_val; i += 2) {
        if (val % i == 0)  // valの約数が見つかった場合
            return false;
    }
    return true;  // どの数でも割り切れない場合は、valは素数
}


int main(int argc, char const *argv[]) {
    const int port_number = 12345;

    // 1. create socket
    int server_fd = 0;
    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error create socket : " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE); // 異常終了
    }
    printf("Server_socket_fd_number : %d\n", server_fd);  // サーバー側のソケットディスクリプタを確認

    // 2. setup socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));  // 構造体 struct sockaddr_in をゼロで初期化
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_number);

    // // (実装は任意) setup socket option
    // int opt = 1;
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    //     std::cerr << "Error set socket : " << strerror(errno) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error bind : " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }


    // 3. start to listen request
    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Error listen : " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "接続を待機しています..." << std::endl;
    }

    // 4. accept request from client
    struct sockaddr_in client;
    int client_len = sizeof(client);
    int new_sock = 0;
    if ((new_sock = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&client_len)) < 0) {
        std::cerr << "Error accept : " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else {
        printf("new_socket_fd_number on Server : %d\n", new_sock);
    }

    // 5. read or write data with client
    
    // 5-1. read data from client
    char buff[1024] = {0};     // データを格納するためのバッファを作成
    recv(new_sock, buff, sizeof(buff), 0);  // ソケットからデータを受信してバッファに格納
    const std::string str = std::string(buff);  // バッファから文字列を作成

    unsigned long val = 0;  // 数値変数を初期化
    std::string msg = "";   // クライアントに送信するメッセージを初期化

    // 受信した文字列を数値に変換し、素数かどうかを判定
    if (StrToValue(val, str)) {
        if (IsPrime(val)) {
            msg = "Prime Number";
        } else {
            msg = "Not Prime Number";
        }
    } else {
        msg = "Please send value!";  // StrToValue関数内で例外処理が行われた場合
    }

    // 5-2. send response to client
    /* send()関数の第二引数には、送信するメッセージが入っているバッファへのポインタしか渡せないため、
        std::stringクラスのc_str()メソッドで、文字列データの先頭アドレスを指す「charのポインタ (char*)」を取り出す */ 
    const char* pmsg = msg.c_str();
    send(new_sock, pmsg, msg.length() + 1, 0);  // string型の文字列データから先頭アドレス
    std::cout << "Received message is \"" << buff << "\"" << std::endl;

    // 6. socket close
    close(new_sock);
    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);
    return 0;
}
