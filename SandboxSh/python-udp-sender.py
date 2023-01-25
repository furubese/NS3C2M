import socket

M_SIZE = 1024
serv_address = ('1.2.3.4', 80)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    try:
        print('')
        message = input()
        if message != 'end':
            send_len = sock.sendto(message.encode('utf-8'), serv_address)
            rx_meesage, addr = sock.recvfrom(M_SIZE)
            print(f"{rx_meesage}")
        else:
            sock.close()
            break

    except KeyboardInterrupt:
        sock.close()
        break
