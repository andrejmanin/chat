#include "interface.h"

#include <iostream>

using namespace Interface;

void Info::start() {
    Colors::info();
    std::cout << "Welcome to Ugram" << std::endl;
    std::cout << "1. Register" << std::endl;
    std::cout << "2. Login" << std::endl;
    std::cout << "Please, choose an option: ";
    Colors::resetColor();
}

void Info::registerUser() {
    Colors::info();
    std::cout << "Enter your username: ";
    std::cin.ignore();
    Colors::resetColor();
    Colors::send();
    std::cout << "Enter your email: ";
    std::cin.ignore();
    Colors::resetColor();
    Colors::send();
    std::cout << "Enter your password: ";
    std::cin.ignore();
    Colors::resetColor();
}

void Info::loginUser() {
    Colors::info();
    std::cout << "Enter your email: ";
    Colors::resetColor();
    Colors::send();
    std::cout << "Enter your password: ";
    Colors::resetColor();
}

void Info::registrationSuccess() {
    Colors::info();
    std::cout << "Registration successful" << std::endl;
    Colors::resetColor();
}

void Info::loginSuccess() {
    Colors::info();
    std::cout << "Login successful" << std::endl;
    Colors::resetColor();
}

void Info::close() {
    Colors::info();
    std::cout << "You can close the terminal" << std::endl;
    Colors::resetColor();
}


void Info::getClients() {
    Colors::info();
    std::cout << "The list of clients is: " << std::endl;
    Colors::resetColor();
}

void Info::sentUsername() {
    Colors::info();
    std::cout << "Username sent to server" << std::endl;
    Colors::resetColor();
}

void Info::info() {
    Colors::info();
    std::cout << "------------------------------------- INFO -------------------------------------" << std::endl;
    std::cout << "1. To send a message to a user, type the following command: 'username: message'" << std::endl;
    std::cout << "2. To get the list of users, type the following command: '/list'" << std::endl;
    std::cout << "3. To open info about the chat, type the following command: '/info'" << std::endl;
    std::cout << "4. To exit the chat, type the following command: '/exit'" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    Colors::resetColor();
}

void Info::msgInfo() {
    Colors::info();
    std::cout << "To send a message to a user, type the following command: 'username: message'" << std::endl;
    Colors::resetColor();
}


void Colors::send() {
    std::cout << "\033[1;32m";
}

void Colors::receive() {
    std::cout << "\033[1;34m";
}

void Colors::info() {
    std::cout << "\033[1;33m";
}

void Colors::error() {
    std::cout << "\033[1;31m";
}

void Colors::resetColor() {
    std::cout << "\033[0m";
}
