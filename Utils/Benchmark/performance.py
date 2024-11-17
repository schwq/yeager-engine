from server import Server

if __name__ == "__main__":
    server = Server("127.0.0.1", 5005)
    message = server.receive()
    print("[CLIENT] -> Says {}".format(message))
    server.send("Server says hello!")

    while True:
        message = server.receive()
        print("[CLIENT] -> Says {}".format(message))
