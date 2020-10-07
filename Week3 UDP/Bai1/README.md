## Exercise 1 Week 3: C program Simple UDP socket server and client
Create a simple UDP socket with server and client.
### Client:
+ Allow user to enter any string from keyboard and send it to server.
+ Receive result from server and display this
+ Function repeat until user enter an empty string

### Server:
+ Reverve string from client
+ Send to Client 2 string: 1 for digit, 1 for alphabet. If client send an string contain character not a digit or a alphabet, send result Error! to Client

## Usage:
```
make
./server portNumber #Ex: ./server 6001
./client IPAddress port Number  #Ex: ./client 127.0.0.1 6001
```
### in Client
```
Insert string to send: abc123
Reply from server:
abc
123
```

```
Insert string to send: abc123@
Reply from server:
Error!
```
```
Insert string to send: (enter)
Goodbye!
```
