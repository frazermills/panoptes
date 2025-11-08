#include <iostream>
#include <fstream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "../../engine/include/DataTypes.h" // Include the message definition

constexpr int UDP_PORT = 12345;
const char* TARGET_IP = "127.0.0.1";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_data_file>" << std::endl;
        return 1;
    }
    const char* data_file_path = argv[1];

    std::ifstream input_file(data_file_path, std::ios::binary | std::ios::ate);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open data file " << data_file_path << std::endl;
        return -1;
    }

    std::streamsize size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!input_file.read(buffer.data(), size)) {
        std::cerr << "Error: Could not read data file into buffer." << std::endl;
        return -1;
    }
    input_file.close();

    const size_t num_messages = size / sizeof(PanoptesMessage);
    std::cout << "Loaded " << num_messages << " messages from " << data_file_path << std::endl;

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        return -1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, TARGET_IP, &server_addr.sin_addr);

    std::cout << "Thrasher is ready. Start the engine now, then press Enter to begin." << std::endl;
    std::cin.get(); // Wait for user to press Enter
    std::cout << "Starting data blast..." << std::endl;

    PanoptesMessage* messages = reinterpret_cast<PanoptesMessage*>(buffer.data());
    for (size_t i = 0; i < num_messages; ++i) {
        // *** THIS IS THE CRITICAL FIX ***
        // Overwrite the historical timestamp with the current time right before sending.
        auto now = std::chrono::high_resolution_clock::now();
        messages[i].timestamp = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

        sendto(sock_fd, &messages[i], sizeof(PanoptesMessage), 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr));
    }

    std::cout << "Finished sending " << num_messages << " messages." << std::endl;
    close(sock_fd);

    return 0;
}
