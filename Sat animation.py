import math
from datetime import datetime, timedelta
from matplotlib import animation
from pyorbital.orbital import Orbital
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as lines
from mpl_toolkits.mplot3d import axes3d


def relative_rotation(time_, coord):
    return np.array([
        np.cos(angular_velocity * time_) * coord[0] - np.sin(angular_velocity * time_) * coord[1],
        np.sin(angular_velocity * time_) * coord[0] + np.cos(angular_velocity * time_) * coord[1],
        coord[2]
    ])

#геоинф геоинф (сверху тоже)
a, b = 6378.137, 6356.7523142
angular_velocity = 7.268 * (10 ** (-5))
start = datetime.now()
ecis = []  # Координаты

sat = Orbital("NOAA 19")
for i in range(1440):
    ecis.append(relative_rotation(i * 60,
                                  sat.get_position(start + timedelta(minutes=i), normalize=False)[0]))
ecis = np.array(ecis)
c_x, c_y, c_z = ecis[:, 0], ecis[:, 1], ecis[:, 2]

#АНИМАЦИЮ НАЧИНАЮ ДЕЛАТЬ ТУТ

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

#отрисовываемые коорды
xdata, ydata, zdata = [], [], []

#Создаем объект Line3D, обновляемый по мере отрисовки новых графиков
# line = axes3d.art3d.Line3D([], [], [])
line3d, = plt.plot([], [], [], color="red")
line3d.set_data_3d([], [], [])

#оболочка (геоинф привет)
rx, ry, rz = a, a, b
u = np.linspace(0, 2 * np.pi, 100)
v = np.linspace(0, np.pi, 100)
x = rx * np.outer(np.cos(u), np.sin(v))
y = ry * np.outer(np.sin(u), np.sin(v))
z = rz * np.outer(np.ones_like(u), np.cos(v))

ax.plot_wireframe(x, y, z, alpha=0.1)

max_radius = max(rx, ry, rz)
for axis in 'xyz':
    getattr(ax, 'set_{}lim'.format(axis))((-max_radius, max_radius))

#Функция, обновляющая Line3D, добавляя координаты в конец массивов xdata, ydata, zdata (коорды на вывод)
def animate(k):
    xdata.append(c_x[k])
    ydata.append(c_y[k])
    zdata.append(c_z[k])
    line3d.set_data_3d(xdata, ydata, zdata)
    return line3d,

#Сама анимация. Использует функцию animate, чтобы обновлять линию, а затем выводит её в fig.
anim = animation.FuncAnimation(fig, animate, frames=500, interval=10, blit=True)

#Сэйвит анимацию в гифке
anim.save("animation30fps.gif", writer=animation.PillowWriter(fps=30))

# plt.show()
