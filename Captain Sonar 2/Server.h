#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <thread>
using boost::asio::ip::tcp;
static std::thread t3;

static std::thread t1;
static std::thread t2;
static bool run1 = true;
std::vector<std::thread> threads;
std::vector<bool> playersReady(3);

void getData(tcp::socket* socketptr, int playerID) {
	tcp::socket& socket = *socketptr;
    std::cout << "Client connected." << std::endl;

	// Send the player ID to the client
    boost::asio::write(socket, boost::asio::buffer(to_string(playerID), 1));

    char data[1024] = { 0 };
    boost::system::error_code error;

	// Wait for the first two players to connect
    int k = 0;
    while (k < 2) {
        k = 0;
        std::size_t length = socket.read_some(boost::asio::buffer(data), error);
        
        playersReady.at(playerID) = atoi(data);

        for (int i = 0; i < playersReady.size(); i++) {
            if (playersReady.at(i) == 1) {
                k++;
            }
        }

		if (k == 2) {
			std::cout << "Starting game..." << std::endl;
			boost::asio::write(socket, boost::asio::buffer("start", 5));
			break;
		}
        boost::asio::write(socket, boost::asio::buffer("not", 3));
    }
    for (int i = 0; i < playersReady.size(); i++) {
        playersReady.at(i) = 0;
    }
    playersReady.at(0) = 1;
    for (;;) {
        using namespace std::this_thread; // sleep_for, sleep_until
        using namespace std::chrono; // nanoseconds, system_clock, seconds

        sleep_until(system_clock::now() + seconds(1));
            if (playersReady.at(playerID) == 1) {
                boost::asio::write(socket, boost::asio::buffer("1", 1));
            }
            else
                boost::asio::write(socket, boost::asio::buffer("0", 1));

        if (playersReady.at(playerID) == 0) continue;
        std::size_t length = socket.read_some(boost::asio::buffer(data), error);
        for (int i = 0; i < playersReady.size(); i++) {
            playersReady.at(i) = !playersReady.at(i);
        }
        std::cout << data << std::endl;
        if (error == boost::asio::error::eof) break; // connection closed cleanly
        else if (error) throw boost::system::system_error(error); // other 
    }
    std::cout << "Client disconnected." << std::endl;
    delete socketptr;
}
void server(int port) {
    try {
        boost::asio::io_context io_context;

        // Listen on port 12345
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "Server started." << std::endl;
        int player = 0;
        while (run1) {
            tcp::socket* socket = new tcp::socket(io_context);
            acceptor.accept(*socket);
            threads.push_back(std::thread(getData, socket, player));
            player++;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void startServer(int port) {
    t3 = std::thread(server, port);
}

void stopServer() {
    run1 = false;
	if (t3.joinable()) {
		t3.join();
	}
	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}
