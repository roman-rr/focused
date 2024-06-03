#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <iterator>
#include <filesystem>
#include <unistd.h>  // for geteuid()

// Function to read lines from a file
std::vector<std::string> read_lines(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        lines.push_back(line);
    }

    return lines;
}

// Function to check if a string is an IP address
bool is_ip_address(const std::string &str) {
    std::regex ip_regex(R"(^(\d{1,3}\.){3}\d{1,3}$)");
    return std::regex_match(str, ip_regex);
}

// Function to block sites
void block_sites(const std::vector<std::string> &sites) {
    std::ofstream hosts_file("/etc/hosts", std::ios_base::app);

    for (const auto &site : sites) {
        std::string site_with_www = (site.find("www.") == 0) ? site : "www." + site;

        if (!is_ip_address(site)) {
            hosts_file << "127.0.0.1 " << site << std::endl;
            hosts_file << "127.0.0.1 " << site_with_www << std::endl;
        } else {
            hosts_file << "127.0.0.1 " << site << std::endl;
        }
    }
}

// Function to unblock sites
void unblock_sites(const std::vector<std::string> &sites) {
    std::ifstream hosts_file_in("/etc/hosts");
    if (!hosts_file_in.is_open()) {
        std::cerr << "Error: Unable to open /etc/hosts for reading." << std::endl;
        return;
    }

    std::ofstream hosts_file_out("/etc/hosts.tmp");
    if (!hosts_file_out.is_open()) {
        std::cerr << "Error: Unable to open /etc/hosts.tmp for writing." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(hosts_file_in, line)) {
        bool block_line = false;

        for (const auto &site : sites) {
            std::string site_with_www = (site.find("www.") == 0) ? site : "www." + site;
            if (line.find(site) != std::string::npos || line.find(site_with_www) != std::string::npos) {
                block_line = true;
                break;
            }
        }

        if (!block_line) {
            hosts_file_out << line << std::endl;
        }
    }

    hosts_file_in.close();
    hosts_file_out.close();

    try {
        std::filesystem::rename("/etc/hosts.tmp", "/etc/hosts");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Function to solve a quadratic equation (simplified for example purposes)
void solve_quadratic() {
    int a = 1, b, c;
    int root1 = rand() % 19 - 9;
    int root2 = rand() % 19 - 9;
    b = -(root1 + root2);
    c = root1 * root2;

    std::cout << "Solve the quadratic equation: " << a << "x^2 + " << b << "x + " << c << " = 0" << std::endl;

    std::cout << "Please enter one of the roots to confirm unblocking: ";
    int user_input;
    std::cin >> user_input;

    if (user_input == root1 || user_input == root2) {
        std::vector<std::string> sites = read_lines("data/distractors.txt");
        unblock_sites(sites);
    } else {
        std::cout << "Incorrect root. Social networks remain blocked." << std::endl;
    }
}

int main() {
    if (geteuid() != 0) {
        std::cerr << "This program must be run as root. Please use 'sudo' to run the program." << std::endl;
        return 1;
    }

    std::vector<std::string> sites = read_lines("data/distractors.txt");
    std::cout << "Do you want to enable or disable blocking social networks? (Enable/Disable): ";
    std::string choice;
    std::cin >> choice;

    std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower);

    if (choice == "enable") {
        block_sites(sites);
    } else if (choice == "disable") {
        solve_quadratic();
    } else {
        std::cout << "Invalid option. Please choose 'Enable' or 'Disable'." << std::endl;
    }

    return 0;
}