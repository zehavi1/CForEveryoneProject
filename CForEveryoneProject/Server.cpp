//#include <iostream>
//#include <cstring>
//#include <winsock2.h> // ספריית סוקטות עבור Windows
//#include <algorithm> // עבור std::reverse
//#include "main.h"
//#pragma comment(lib, "ws2_32.lib") // קישור לספריית winsock
//
//#define PORT 8080
//
//int main3() {
//    WSADATA wsaData;
//    SOCKET server_fd, new_socket;
//    struct sockaddr_in address;
//    int opt = 1;
//    int addrlen = sizeof(address);
//    char buffer[1024] = { 0 };
//
//    // אתחול Winsock
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        std::cerr << "error in sarting Winsock" << std::endl;
//        return -1;
//    }
//
//    // יצירת סוקט
//    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
//        std::cerr << "we didnt success make a socket" << std::endl;
//        WSACleanup();
//        return -1;
//    }
//
//    // הגדרת אפשרויות הסוקט
//    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
//
//    // הגדרת כתובת השרת
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons(PORT);
//
//    // קישור הסוקט לכתובת
//    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
//        std::cerr << "error in link" << std::endl;
//        closesocket(server_fd);
//        WSACleanup();
//        return -1;
//    }
//
//    // הקשבה לחיבורים נכנסים
//    if (listen(server_fd, 3) == SOCKET_ERROR) {
//        std::cerr << "error listening" << std::endl;
//        closesocket(server_fd);
//        WSACleanup();
//        return -1;
//    }
//
//    // קבלת חיבור נכנס
//    addrlen = sizeof(address);
//    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
//        std::cerr << "error connect" << std::endl;
//        closesocket(server_fd);
//        WSACleanup();
//        return -1;
//    }
//
//    // קריאת נתונים מהלקוח
//    recv(new_socket, buffer, 131072, 0);
//    std::cout << "received: " << buffer << std::endl;
//
//    std::string newStr = mainCompiler(buffer);
//
//    // החזרת התגובה ללקוח
//    send(new_socket, newStr.c_str(), newStr.length(), 0);
//
//    // סגירת הסוקט
//    closesocket(new_socket);
//    closesocket(server_fd);
//    WSACleanup();
//    return 0;
//}
