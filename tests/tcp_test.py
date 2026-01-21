import socket

HOST = "127.0.0.1"
PORT = 1234

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    message = b"Hello World!"

    s.sendall(message)

    response = s.recv(4096)

    print("Recieved:", response.decode())
