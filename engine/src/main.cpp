#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <chrono>
#include <vector>
#include <memory>

#include "L1CacheBook.h"
#include "BinaryParser.h"

constexpr int UDP_PORT = 12345;
constexpr int MAX_EVENTS = 10;
constexpr int BUFFER_SIZE = 1024;
constexpr int TIMEOUT_MS = 2000; // 2 seconds

int main() {
    auto book = std::make_unique<L1CacheBook>();
    std::cout << "Project Panoptes Engine Initializing..." << std::endl;

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        return -1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);

    if (bind(sock_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sock_fd);
        return -1;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1 failed");
        close(sock_fd);
        return -1;
    }

    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = sock_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
        perror("epoll_ctl failed");
        close(sock_fd);
        close(epoll_fd);
        return -1;
    }

    epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];
    std::vector<int64_t> latencies;
    latencies.reserve(1000000); // Pre-allocate for performance

    std::cout << "Engine is listening on port " << UDP_PORT << std::endl;
    long message_count = 0;
    bool stream_started = false;

    while (true) {
        // Wait for packets, but with a timeout.
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT_MS);

        if (num_events == 0) {
            // This means epoll_wait timed out.
            if (stream_started) {
                std::cout << "No packets received for " << TIMEOUT_MS << "ms. Assuming stream has ended." << std::endl;
                break; // Exit the loop to print the summary.
            }
            continue; // If stream never started, keep waiting.
        }

        if (num_events < 0) {
            perror("epoll_wait failed");
            break;
        }

        stream_started = true; // Mark that we've received at least one packet.

        for (int i = 0; i < num_events; ++i) {
            int bytes_read = recv(events[i].data.fd, buffer, BUFFER_SIZE, 0);

            if (bytes_read > 0) {
                PanoptesMessage msg = parseMessage(buffer);
                auto now = std::chrono::high_resolution_clock::now();
                int64_t now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

                switch (msg.event_type) {
                    case 'A': book->addOrder(msg); break;
                    case 'X': book->cancelOrder(msg); break;
                    case 'E': book->executeOrder(msg); break;
                }

                int64_t latency = now_ns - msg.timestamp;
                if (latency > 0 && latency < 1000000) { // Filter out outliers
                    latencies.push_back(latency);
                }

                message_count++;
            }
        }
    }

    close(sock_fd);
    close(epoll_fd);

    // This code is now reachable after the loop breaks.
    long long total_latency = 0;
    for (long long l : latencies) {
        total_latency += l;
    }

    std::cout << "------------------------------------------" << std::endl;
    std::cout << "           PERFORMANCE SUMMARY" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Total messages processed: " << message_count << std::endl;
    if (!latencies.empty()) {
        std::cout << "Average latency: " << (double)total_latency / latencies.size() << " ns" << std::endl;
    }
    std::cout << "------------------------------------------" << std::endl;

    return 0;
}
