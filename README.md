# Convo-Messenger

The world first whatsapp like app without Javascript

## Preview

![image1](https://github.com/EDBCREPO/HTTPSocket/blob/main/images/image1.gif?raw=true)
![image2](https://github.com/EDBCREPO/HTTPSocket/blob/main/images/image2.gif?raw=true)

## Features

- It is memory safety thanks to NodePP 
- It is complitly Asinchronous no pthread.
- The Server Knows when Client is connected.
- The Server Knows when Client is disconnected.
- The server suppots Poll, Epoll, WSAPoll, Kqueue.
- It is Compatible with Windows, Linux, Mac & FreeBSD.

## Dependencies

- **redis-cli:** [redis.io](https://redis.io/docs/latest/develop/use/client-side-caching/)

## Build & Usage

```bash
💻: git clone https://github.com/ConvoOfficial/Convo
🪟: time g++ -o main main.cpp -I ./include -lssl -lcrypto -lz -lsqlite3 -lws2_32 ; ./main
🐧: time g++ -o main main.cpp -I ./include -lssl -lcrypto -lz -lsqlite3 ; ./main
```

## How does it works

It is so simple, there are two things we need the browser to do. Send Data and Receive Data. Let's start with the first.

## The Final Result

![image3](https://github.com/EDBCREPO/HTTPSocket/blob/main/images/image3.png?raw=true)