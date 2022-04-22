import socket
import time
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
client_ID = "1"

# container for animation (JUST FOR 1 SAT!!!)
coords_container = [[np.nan, np.nan, np.nan]]

def animate(i):
    data = np.array(coords_container)
    ax.clear()
    ax.set_xlim(-1e7, 1e7)
    ax.set_ylim(-1e7, 1e7)
    ax.set_zlim(-1e7, 1e7)
    ax.plot(data[:, 0], data[:, 1], data[:, 2], c="r")
    ax.scatter(data[-1, 0], data[-1, 1], data[-1, 2], c="black", marker="H")


# pushes new coords into container for animation
def push_coords(r):
    if len(coords_container) < 30:
        coords_container.append(r)
    else:
        coords_container.pop(0)
        coords_container.append(r)
    # print(coords_container)


def analyse_data(data):
    # push is only added, if get coords cont function was called (see sat code)
    # better to add push on server
    if data[0:4] == "push":
        r_str = data[5:].split(" ")
        r = [float(r_str[0]), float(r_str[1]), float(r_str[2])]
        push_coords(r)


# here we listen to server responds
def client_handler():
    global connected
    while connected:
        data_len = sock.recv(1)  # try 1 and 2 else
        lengh_rcv = ord(data_len)
        time.sleep(0.1)

        data = sock.recv(lengh_rcv + 3)
        print(data.decode('utf-8')[3:])
        data_str = data.decode('utf-8')[3:]
        analyse_data(data_str)
        # first 3 bite's are not message
        time.sleep(0.1)


# here we can type and send requests on server
def client_input():
    global connected
    if connected:
        msg = "help"
    while connected:
        msg = str(input())
        length = chr(len(msg))
        if msg == "disconnect":
            sock.send(length.encode('utf-8'))
            time.sleep(0.01)
            sock.send(msg.encode('utf-8'))
            time.sleep(0.01)
            connected = 0
        else:
            sock.send(length.encode('utf-8'))
            time.sleep(0.01)
            sock.send(msg.encode('utf-8'))
            time.sleep(0.01)
    print("Bye!")


sock = socket.socket()
sock.connect(('localhost', 1111))
connected = 1
sock.send("1".encode('utf-8'))
sock.send(client_ID.encode('utf-8'))

x = threading.Thread(target=client_handler, args=())
print("Client connected!")
x.start()

y = threading.Thread(target=client_input, args=())
y.start()
print("Commands available:\n1. 'get coords' - get coordinates\n"
      "2. 'get coords cont' - get coordinates continuously")
print("Type 'refresh' to see available satellites:")

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ani = animation.FuncAnimation(fig, animate, interval=200)
plt.show()
# socket.close()
