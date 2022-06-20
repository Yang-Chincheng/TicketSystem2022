#!/usr/bin/env python

import socket

HOST = '127.0.0.1'
PORT = 5000

def call_cpp(str):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((HOST, PORT))
        sock.sendall(str)
        res = sock.recv(4096)
    return res

def main():
    while True:
        cmd = input()
        ret = call_cpp(cmd)
        print('Recv:', repr(ret))

if __name__ == '__main__':
    main()
