# Convo-Messenger

Introducing the revolutionary messaging app that breaks free from the constraints of Javascript. Inspired by [ConvoOfficial/HTTPSocket](https://github.com/ConvoOfficial/HTTPSocket) the world's first WhatsApp-like application! Say goodbye to the limitations of Javascript and experience seamless, lightning-fast messaging without compromise. Our innovative technology ensures a smooth user experience, allowing you to connect with friends, family, and colleagues in real-time without the need for Javascript. Embrace a new era of messaging with our cutting-edge app, designed to deliver unparalleled performance and reliability without sacrificing functionality. Join the movement and discover a messaging experience like never before - all without Javascript.

## Preview

### Convo Messenger
![image1](https://github.com/ConvoOfficial/Convo-Messenger/blob/main/Preview/image1.gif?raw=true)

### Convo Login
![image2](https://github.com/ConvoOfficial/Convo-Messenger/blob/main/Preview/image2.png?raw=true)

### Convo Messenger
![image3](https://github.com/ConvoOfficial/Convo-Messenger/blob/main/Preview/image3.png?raw=true)

## Features

- 📌: It is memory safe thanks to [The Nodepp Project](https://github.com/NodeppOficial/nodepp), So you don have to worry about Segment Fault Errors.
- 📌: It is complitly Asinchronous, No Thread Pool, Just Non-blocking Programming, So you can Handle a lot of users at the same time.
- 📌: It notify the client when there is a new message or the other peer is 🟢Connected / 🔴Disconnected.
- 📌: No Javascript Needed, Just Html, Css & C++.

## Dependencies
- **Openssl**
    - 🪟: `pacman -S mingw-w64-ucrt-x86_64-openssl`
    - 🐧: `sudo apt install libssl-dev`

- **Zlib**  
    - 🪟: `pacman -S mingw-w64-ucrt-x86_64-zlib`
    - 🐧: `sudo apt install zlib1g-dev`

- **Redis**
    - 💻: [**redis.io/install**](https://redis.io/docs/latest/operate/oss_and_stack/install/install-redis/)

## Build & Usage

```bash
# Clone the Project
💻: git clone https://github.com/ConvoOfficial/Convo-Messenger

# Clone the Submodules
💻: git submodule update --init --recursive

# Compile the Code
🐧: time g++ -o main main.cpp -I ./Modules -lssl -lcrypto -lz ; ./main
🪟: time g++ -o main main.cpp -I ./Modules -lssl -lcrypto -lz -lws2_32 ; ./main
```

## Contribution

If you want to contribute to **Convo**, you are welcome to do so! You can contribute in several ways:

- ☕ Buying me a Coffee
- 📢 Reporting bugs and issues
- 📝 Improving the documentation
- 📌 Adding new features or improving existing ones
- 🧪 Writing tests and ensuring compatibility with different platforms
- 🔍 Before submitting a pull request, make sure to read the contribution guidelines.

## License

**Nodepp** is distributed under the MIT License. See the LICENSE file for more details.
