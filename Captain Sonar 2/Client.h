#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "Player.h"

Player player1;
Player player2;

using boost::asio::ip::tcp;
static std::thread t;
static bool run = true;
int ID;
bool ready = false;
int players = 0;
bool readyup = false;

bool startGame = false;

bool isTurn = false;
void client(std::string ip, std::string port) {
    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(ip.c_str(), port.c_str());

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Get the playerID
        char playerID[1024];
        std::size_t reply_length = socket.read_some(boost::asio::buffer(playerID));
        ID = atoi(playerID);
        std::cout << "Reply from server: " << ID;
        std::cout << std::endl;
        char data[1024] = { 0 };

		// Wait for the first two players to connect
		while (std::string(data) != "start") {
            boost::asio::write(socket, boost::asio::buffer(to_string(readyup)));
            std::size_t reply_length = socket.read_some(boost::asio::buffer(data));
           
		}
        std::cout << "starting game" << std::endl;
        startGame = true;

        while (true) {
            char data[1024] = { 0 };
            std::size_t reply_length = socket.read_some(boost::asio::buffer(data));
            std::cout << atoi(data) << std::endl;
            if (atoi(data)) {
                while (!ready) {
                    isTurn = true;
                    std::string message = to_string(ID) + " " + to_string(player1.position.x) + " " + to_string(player1.position.y);
                    if (ready) {
                        boost::asio::write(socket, boost::asio::buffer(message));
                    }
                }
                ready = false;
            }
            else {
                isTurn = false;
                ready = false;
            }
        }

    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

  
}

void connect(std::string ip, std::string port) {
    t = std::thread(client, ip, port);
}