import pygame as pg
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import sys
from socket import *
import numpy as np
from time import time, strftime
from numpy import frombuffer, array, savetxt
from functools import partial
from datetime import datetime
 


# Create a UDP socket
s = socket(AF_INET, SOCK_DGRAM)
port = 9990
# Assign IP address and port number to socket
s.bind(('192.168.0.16', port))
print('server bind at ', s)
BUFFER_SIZE = 1024 

Vertices = ((1, 1, 1), (1, 1, -1), (1, -1, -1), (1, -1, 1), (-1, 1, 1), (-1, -1, -1), (-1, -1, 1), (-1, 1, -1))
Edges = ((0, 1), (0, 3), (0, 4), (1, 2), (1, 7), (2, 5), (2, 3), (3, 6), (4, 6), (4, 7), (5, 6), (5, 7))
Quads = ((0, 3, 6, 4), (2, 5, 6, 3), (1, 2, 5, 7), (1, 0, 4, 7), (7, 4, 6, 5), (2, 3, 0, 1))


def wireCube():
    glBegin(GL_LINES)
    for cubeEdge in Edges:
        for vertex in cubeEdge:
            glVertex3fv(Vertices[vertex])
    glEnd()


def client(s, txt):
    while True:
        data, addr = s.recvfrom(BUFFER_SIZE)
        if data:
            ct = str(datetime.now())[:19]
            msg = ct + '>>' + addr[0] + ': ' + str(frombuffer(data, dtype = np.uint8))
            txt.insert(INSERT, msg + '\n')
            txt.see(END)
        
def ack(n, addr):
    nack = array([n], dtype = '<u1')
    s.sendto(nack.tobytes(), addr)

def clear():
    txt.delete(1.0, END)

log_file = open("gyroscope_data.txt", "w")    

pg.init()
display = (800, 600)
pg.display.set_mode(display, DOUBLEBUF|OPENGL)
gluPerspective(45, (display[0]/display[1]), 0.1, 50.0)
glTranslatef(0.0, 0.0, -8)  #move cube farther away, increase z

clock = pg.time.Clock()

angle_x = 0
angle_y = 0
prev_roll = 0
prev_pitch = 0
roll = 0
pitch = 0
val = 0


try:
    while True:
        for event in pg.event.get():
            if event.type == pg.QUIT:
                pg.quit()
                sys.exit()
                
        data, addr = s.recvfrom(BUFFER_SIZE)

        if len(data) == 5:
            roll = int.from_bytes(data[0:2], byteorder = 'little', signed = True)
            pitch = int.from_bytes(data[2:4], byteorder = 'little', signed = True)
            val = data[4]
        
        
            log_string = f"Roll: {roll}, Pitch: {pitch}, Counter: {val}\n"
            log_file.write(log_string)
            print(log_string)
    
        print(f"Roll: {roll}, Pitch: {pitch}, Counter: {val}")
        rot_speed_x = abs(2 * (roll - prev_roll))/3
        rot_speed_y = abs(2 * (pitch - prev_pitch))/3
    
        if roll > prev_roll:
            angle_x += rot_speed_x
    
        elif roll < prev_roll:
            angle_x -= rot_speed_x
        
        if pitch > prev_pitch:
            angle_y += rot_speed_y
        
        elif pitch < prev_pitch:
            angle_y -= rot_speed_y
    
        prev_roll = roll
        prev_pitch = pitch
    
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    
        glPushMatrix()
        glRotatef(angle_x, 1, 0, 0)
        glRotatef(angle_y, 0, 1, 0)
    
        wireCube()
    
        glPopMatrix()
    
        pg.display.flip()
        pg.time.wait(20)
finally:
    log_file.close()

