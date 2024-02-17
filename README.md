# Chat-tool
An “IPv4 chat” program that exchanges text broadcast (IPv4 UDP broadcast) messages over a local network segment, with copies running on other machines physically connected to the same logical network segment.

# Overview

This is a simple chat application implemented in C++. It exchanges text broadcast (IPv4 User Datagram Protocol broadcast) messages over a local network segment with copies running on other machines physically connected to the same logical network segment.


# Usage

1. Building the application:
   - (Install CMake).
   - Navigate to the root directory of the project.
   - Run the following commands:
  
     ``` 
     mkdir build
     cd build
     cmake ..
     make
     ```

2. Starting the chat server and client:
   - Run the following command:
     ``` 
     ./chat <IP Address> <port>
     ```
   * Replace <port> and <IP address> with the desired numbers for the server.

3. Chatting:
   - Once the client is connected to the server, enter your desired nickname when asked.


# Dependencies

- CMake
- C++ compiler (GCC for Linux, Visual C++ for Windows)

# Contributing

Contributions are welcome! If you'd like to contribute to the project, please fork the repository, make your changes, and submit a pull request.
