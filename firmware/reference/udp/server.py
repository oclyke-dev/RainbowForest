import socket

PORT = 5678
BUFFER_SIZE = 1024

host = socket.gethostbyname(socket.gethostname())
print(f'server ip address: {host}')

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((host, PORT))

while True:
  data, client_addr = s.recvfrom(BUFFER_SIZE)
  
  if data:
    print(f'Client ({client_addr}) to Server: {data}')
    s.sendto(data.upper(), client_addr)

s.close()
