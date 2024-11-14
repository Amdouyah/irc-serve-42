# ft_IRC (Internet Relay Chat)

## Overview
This project involves creating an IRC (Internet Relay Chat) server in C++98. The server implements the IRC protocol, allowing multiple clients to connect, communicate, and manage channels.

## Features
- TCP/IP socket communication
- Multiple client handling using non-blocking I/O
- Channel management (create, join, leave)
- Private messaging between users
- User authentication
- Basic IRC commands implementation
- Password protection for server access

## Mandatory Commands Implemented
- `/nick`: Set or change nickname
- `/join`: Join a channel
- `/part`: Leave a channel
- `/quit`: Disconnect from server
- `/privmsg`: Send private message to user or channel
- `/topic`: Set or view channel topic
- `/mode`: Change channel or user modes
- `/kick`: Remove user from channel
- `/invite`: Invite user to channel

## Prerequisites
- C++ compiler (supporting C++98)
- Make
- Linux/Unix environment

## Building the Project
1. Clone this repository:

    ```bash
    git clone https://github.com/Amdouyah/inception.git
    cd inception
    ```
2. **Build and Deploy**:

    ```bash
    $ make
    ```
3. **Starting the Server**
   ```bash
    $ ./ircserv <port> <password>
   ```
   . **port**: Port number on which the server will listen
   . **password**: Password for server access
5. **Connecting with a Client**
   ```bash
     $ nc localhost <port>
     $ # Then enter the password when prompted
   ```
## Implementation Details
  - Written in C++98 standard
  - Uses TCP/IP sockets for communication
  - Implements non-blocking I/O with select()
  - Single-threaded design
  - No external libraries except standard C++98

## Error Handling
  - Invalid commands
  - Connection issues
  - Authentication failures
  - Permission errors
  - Channel operation errors
**Testing**
  -you can use **HexChat** or **nc**
## Acknowledgments
- 42 school
- [Modern IRC Documentation](https://modern.ircdocs.horse/) - For comprehensive IRC protocol understanding
- Peer-to-peer learning and collaboration with fellow 42 students
  
