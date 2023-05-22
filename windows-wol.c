// This file is to build wol program with fixed mac and broadcast address
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create a UDP socket
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Enable broadcast option for the socket
    BOOL broadcastOption = TRUE;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastOption, sizeof(broadcastOption)) == SOCKET_ERROR) {
        std::cerr << "Failed to set socket option." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Prepare the magic packet by repeating the target device's MAC address 16 times
    std::string macAddress = "54:e1:ad:ad:c4:d2";  // Replace with the MAC address of the target device
    std::string magicPacket;
    for (int i = 0; i < 16; ++i) {
        std::string byte;
        for (int j = 0; j < 6; ++j) {
            byte += std::stoi(macAddress.substr(j * 3, 2), nullptr, 16);
        }
        magicPacket += byte;
    }

    // Set the target IP address and port
    SOCKADDR_IN targetAddress;
    targetAddress.sin_family = AF_INET;
    targetAddress.sin_port = htons(9);  // Default WoL port is 9
    targetAddress.sin_addr.s_addr = inet_addr("192.168.0.255");  // Use the broadcast address

    // Send the magic packet
    if (sendto(sock, magicPacket.c_str(), magicPacket.size(), 0, (SOCKADDR*)&targetAddress, sizeof(targetAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to send WoL packet." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "WoL packet sent successfully!" << std::endl;

    // Cleanup
    closesocket(sock);
    WSACleanup();

    return 0;
}

