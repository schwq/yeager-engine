import numpy as np
import time
import socket


class Server:
    def __init__(self, ip, port, automatic_port=True):
        self.message_size_lenght = 16
        max_connections = 5

        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if automatic_port:
            connected = False
            while not connected and (max_connections > 0):
                try:
                    self.s.bind((ip, port))
                    connected = True
                except:
                    print(
                        "[SERVER] -> Port {} already in use. Binding to port: {}".format(
                            port, port + 1
                        )
                    )
                    port += 1
                    max_connections -= 1
                if not connected:
                    print("[SERVER] -> Cannot binding to address!")
        else:
            self.s.bind((ip, port))

        self.s.listen(True)
        print("[SERVER] -> Awaiting for connection...")
        self.conn, addr = self.s.accept()
        print("[SERVER] -> Connected!")

    def __del__(self):
        self.s.close()

    def send(self, message):
        message_size = str(len(message)).ljust(self.message_size_lenght).encode()
        self.conn.sendall(message_size)
        self.conn.sendall(message.encode())

    def receive(self, decode=True):
        lenght = self.receive_value(self.conn, self.message_size_lenght)
        if lenght is not None:
            message = self.receive_value(self.conn, int(lenght), decode)
            return message
        return None

    def receive_value(self, conn, buf_lenght, decode=True):
        buf = b""
        while buf_lenght:
            newbuf = conn.recv(buf_lenght)
            buf += newbuf
            buf_lenght -= len(newbuf)
        if decode:
            return buf.decode()
        else:
            return buf

    def clear_buf(self):
        try:
            while self.conn.recv(1024):
                pass
        except:
            pass
        return
