# Gameplan: Make a Galaga type game

# Step 1: Make the game in pygame using only keyboard control first
#       progress 3/19/23: completed the basics of the game
#           - Also might implement scoring at a later date
#           - Also might implement different shapes later (i.e. enemies are triangles not squares)

# Step 2:   - BLE does not seem to be working for Mac so for now I will use MQTT
#           - Try to get MQTT connected today (3/22/23) 
#           - Work on accelometer controls later
#           - Figure out how to do threading with MQTT

import paho.mqtt.client as mqtt
# MQTT settings
MQTT_BROKER = "info8000.ga"
MQTT_PORT = 1883
MQTT_USER = "giiuser"
MQTT_PW = "giipassword"
MQTT_CLIENT_ID = "ugaelee2045sp23/tstpierre/BTcontrol"
T_CONTROL_FIRED = "ugaelee2045sp23/tstpierre/BTcontrol_shot_fired"
T_CONTROL_ACC = "ugaelee2045sp23/tstpierre/BTcontrol_accelerometer"

def on_message(client_obj, userdata, message): #note, userdata is unused
    print(f"Message received: {message.payload}")
    # decode the message
    global byte0
    global byte1
    global byte2
    global byte3
    byte0 = message.payload[0]
    byte1 = message.payload[1]
    byte2 = message.payload[2]
    byte3 = message.payload[3]
    print(f"button: {byte0}, x:{byte1}, y:{byte2}, z:{byte3}")

# create the MQTT client and connect to the broker
client = mqtt.Client(MQTT_CLIENT_ID)
client.username_pw_set(MQTT_USER, MQTT_PW)
client.on_message = on_message
client.connect(MQTT_BROKER, MQTT_PORT)
client.subscribe(MQTT_CLIENT_ID)
client.subscribe(T_CONTROL_FIRED)
client.subscribe(T_CONTROL_ACC)
client.loop_start()

import pygame
import random

pygame.init()

clock = pygame.time.Clock()

screen_width = 800
screen_height = 600

screen = pygame.display.set_mode((screen_width,screen_height))
black = 0,0,0
dark_gray = 55,55,55
red = 255,0,0
green = 0,255,0
blue = 0,0,255

class Rectangle:
    def __init__(self, color, x, y, width, height):
        self.rect = pygame.Rect(x, y, width, height)
        self.color = color
        self.direction = 'East'
        self.speed = 2

    def move(self):
        # Controls here are very weird and determined by testing
        # Accelerometer data through MQTT is very bad and only reads if I move the M5 really fast
        # For example: If I move the M5 to the right fast accX will register 255 and left will register between 1 and 10 for some reason
        if self.direction == 'South' or 100 < byte2 < 255:
            self.rect.y = self.rect.y+self.speed
        elif self.direction == 'West' or 100 < byte1 < 255:
            self.rect.x = self.rect.x-self.speed
        elif self.direction == 'East' or 0 < byte1 < 10:
            self.rect.x = self.rect.x+self.speed
        elif self.direction == 'North' or 0 < byte2 < 10:
            self.rect.y = self.rect.y-self.speed

    def collision(self, rect2):
        # tests if two rects collide
        return self.rect.colliderect(rect2)
    
    def draw(self, surface):
        pygame.draw.rect(surface, self.color, self.rect)
   
# Build the player
r = Rectangle(blue, 400, 500, 40, 40)

shooters = []
enemies = []

finished = False
while not finished:
    if byte0 == 1:
        #shoot a shot
        shooter_start = r.rect.x + r.rect.width/2 - 10
        s = Rectangle(red, shooter_start, r.rect.y, 5, 20)
        s.direction = 'North'
        s.speed = 1
        shooters.append(s)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            finished = True

        # Key Board Control (implement M5 acceleometer controls here later)
        # Update: - we have accelerometer data now we have to figure out how to implement down here
        #         - I would like to have both acc data and keyboard control
        elif event.type == pygame.KEYDOWN:
            if event.key == 119 : #w
                r.direction = 'North'
            if event.key == 97: #a
                r.direction = 'West'
            if event.key == 115: #s
                r.direction = 'South'
            if event.key == 100: #d
                r.direction = 'East'
            if event.key == 32: #spacebar and button pressed
                #shoot a shot
                shooter_start = r.rect.x + r.rect.width/2 - 10
                s = Rectangle(red, shooter_start, r.rect.y, 5, 20)
                s.direction = 'North'
                s.speed = 10
                shooters.append(s)
    # Updating objects
    for s in shooters:
        s.move()

    for e in enemies:
        e.move()

    # better reaction time if move is before drawing
    r.move()

    # Boundary control
    if r.rect.x > screen_width:
        finished = True
    if r.rect.x < 0:
        finished = True
    if r.rect.y > screen_height:
        finished = True
    if r.rect.y < 0:
        finished = True

    # Basically allows enemies to generate every 2 seconds
    # They will generate at random places at the top of the screen and move down
    if random.randint(1,60) == 3:
        x = random.randint(0, screen_width-30)
        rand_width = random.randint(10, 100)
        rand_height = random.randint(10, 100)
        e = Rectangle(green, x, -30 , rand_width, rand_height)
        e.direction = 'South'
        enemies.append(e)

    # Collision check
    for s in shooters:
        for e in enemies:
            if s.collision(e.rect):
                enemies.remove(e)
                shooters.remove(s)
    for e in enemies:
        if r.collision(e.rect):
            finished = True

    # Drawing
    screen.fill(dark_gray)

    # note: drawing must go between ereasing the screen and updating it
    for s in shooters:
        s.draw(screen)

    for e in enemies:
        e.draw(screen)

    r.draw(screen)
    
    #Updating Screen
    pygame.display.flip()

    clock.tick(60)
pygame.quit()
exit()
