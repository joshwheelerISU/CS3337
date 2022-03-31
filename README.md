# IRC DELIVERABLE ASSIGNMENT

This is a basic IRC Server / Client set capable of facilitating conversation between remote clients. It uses sockets and POSIX threads for networking and multithreading, respectively.

## COMPILATION

Use GCC to compile the programs

```bash
gcc server.c -o server.run -lpthread

gcc client.c -o client.run -lpthread
```

## USAGE

SERVER 

```bash
./server -p [portno]
```
### OR
```bash
./server
```

CLIENT

```bash
./client.run [address] [portno] 
```
