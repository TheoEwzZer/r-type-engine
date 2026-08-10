# 🚀 R-Type: Advanced Networked Multiplayer Game Engine

## Quick Navigation

- [Developer Guide](/docs/developer-guide.md) - Technical documentation for developers
- [Network Protocol](/docs/network-protocol.txt) - Detailed protocol specification
- [Technical Comparative Study](/docs/ComparativeStudy.md) - Comparative study
- [License](/docs/license.md) - MIT License

## 🎮 Project Overview

[R-Type](https://fr.wikipedia.org/wiki/R-Type) is an innovative C++ game development project that reimagines the classic horizontal shoot'em up (Shmup) genre. Built with a strong focus on software engineering, networking, and game engine design, this project aims to create a robust, extensible multiplayer game framework.

## 🚀 Quick Start

### Prerequisites

- [CMake](https://cmake.org/) (3.15 or higher)
- [C++20](https://visualstudio.microsoft.com/fr/vs/features/cplusplus/) compatible compiler
- [SFML](https://www.sfml-dev.org/index-fr.php) 2.5.1+
- [Asio](https://think-async.com/Asio/)

### Building from Source

1. Clone the repository:

```bash
git clone git@github.com:EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-theo.fabiano.git
cd r-type
```

2. Build the project:

```bash
./build.sh
```

3. Run the server:

```bash
./r-type_server
```

4. Run the client:

```bash
./r-type_client
```

### Basic Game Controls

- Movement: ZQSD or left Joystick (configurable)
- Shoot: Space bar or 'A' button
- Charge Shot: Hold shoot button

Note: all of the settings can be changed in the settings menu

### Network Configuration

- Default IP: 127.0.0.1
- Default Port: 4242
- Max Players: 4

## 🔧 Troubleshooting

Common issues and solutions:

### Build Fails

- Ensure CMake 3.15+ is installed
- Check SFML dependencies
- Verify compiler supports C++20

### Network Issues

- Check firewall settings
- Verify port 4242 is available

## 🌟 Key Features

- 🌐 **Networked Multiplayer Game Engine**
- 🧵 **Multi-threaded Server Architecture**
- 📡 **Binary UDP-based Communication Protocol**
- 🧩 **Modular Game Engine Design**
- 💻 **Cross-Platform Compatibility** (Linux and Windows)
- 🎵 **Dynamic Audio System** with music and sound effects

## 🔧 Technical Specifications

### 🖥️ Server Characteristics

- Multithreaded implementation
- Authoritative game logic
- Multiple game instance support
- Robust error handling and client disconnection management

### 🎮 Client Characteristics

- Graphical rendering
- Handles player inputs
- Synchronizes with server game state

### 🌐 Networking Protocol

- Binary UDP communication protocol
- Supports multiple game instances
- Implements network reliability mechanisms

## 📋 Technical Requirements

### Build Ecosystem

- [CMake](https://cmake.org/) - Cross-platform build system
- [Build.sh](https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-theo.fabiano/blob/doc/build.sh) - File to execute for building the project

### Platforms

- 🐧 **Primary Platform:** Linux
- 🪟 **Secondary Platform:** Windows

### Libraries

- [SFML](https://www.sfml-dev.org/index-fr.php) - Client rendering
- [Asio](https://think-async.com/Asio/) - Optional networking

## 🏗️ Game Engine Architecture

Implemented using the **Entity-Component-System (ECS)** architectural pattern:

- Decoupled subsystems
- Modular design
- Type-safety at compile-time

## 📚 Documentation

### Required Documentation

- README
- [Developer_Documentation](/docs/developer-guide.md)
- [Technical Comparative Study](/docs/ComparativeStudy.md)
- [Protocol_Documentation](network-protocol.txt) (RFC-style)
- Accessibility Considerations
- License

## ♿ Accessibility Considerations

Designed to support accessibility for:

- Physical and Motor Disabilities
- Audio and Visual Disabilities
- Mental and Cognitive Disabilities

## 👥 Authors

- Theo FABIANO
- Theo MAESTRACCI
- Matthieu BOUSQUET
- Thomas VIDAL SAVELLI

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](/docs/license.md) file for details.

## 🤝 Contribution

Please refer to the developer documentation for contribution guidelines and coding conventions.

## 📞 Contact

For more information, please reach out to the project maintainers.

---

**Inspired by the classic R-Type arcade game, reimagined for the modern era of networked multiplayer gaming.**
