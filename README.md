#**实现TCP服务器与客户端代码**

-------------
- msghdr_realize文件夹中客户端和服务器使用sendmsg()和recvmsg()函数发送数据，使用多进程实现多客户访问服务

- send_recv_realize文件夹中客户端和服务器s使用read()和write()函数发送数据，使用多进程实现多客户访问服务器

- select_realize文件夹中使用多路复用实现多客户访问服务器

- raw_socket_realize 文件夹中使用原始套接字实现对网络层或者数据链路层的分组的嗅探
