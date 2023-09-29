import tkinter as tk
import paho.mqtt.client as mqtt

# MQTT settings
MQTT_BROKER = "info8000.ga"
MQTT_PORT = 1883
MQTT_USER = "giiuser"
MQTT_PW = "giipassword"
MQTT_CLIENT_ID = "m5stickcplus_lightbulb"
T_CONTROL_STATUS = "tvs60497/light_control_status"
T_CONTROL_COLOR = "tvs60497/light_control_color"

def on_message(client_obj, userdata, message): #note, userdata is unused
    print(f"Message received: {message.payload}")

# create the MQTT client and connect to the broker
client = mqtt.Client(MQTT_CLIENT_ID)
client.username_pw_set(MQTT_USER, MQTT_PW)
client.on_message = on_message
client.connect(MQTT_BROKER, MQTT_PORT)

client.subscribe(T_CONTROL_STATUS)
client.subscribe(T_CONTROL_COLOR)
client.loop_start()

# function to handle the "On" and "Off" buttons
def on_off(value):
    # send the value to the M5StickC Plus via MQTT
    client.publish(T_CONTROL_STATUS, bytearray([value]))

# function to handle the color selection
def select_color():
    # get the values from the sliders
    red = red_slider.get()
    green = green_slider.get()
    blue = blue_slider.get()
    color_data = bytes([red, green, blue])
    client.publish(T_CONTROL_COLOR, color_data)
    color_box.config(bg="#{:02x}{:02x}{:02x}".format(red, green, blue))

# create the main window
root = tk.Tk()

root.title("Smart Light")

# create the "On" button
on_button = tk.Button(root, text="On", command=lambda: on_off(1))
on_button.pack()

# create the "Off" button
off_button = tk.Button(root, text="Off", command=lambda: on_off(0))
off_button.pack()

# create the sliders for selecting the color
red_slider = tk.Scale(root, from_=0, to=255, orient=tk.HORIZONTAL, label="Red", length=300)
red_slider.pack()
green_slider = tk.Scale(root, from_=0, to=255, orient=tk.HORIZONTAL, label="Green", length=300)
green_slider.pack()
blue_slider = tk.Scale(root, from_=0, to=255, orient=tk.HORIZONTAL, label="Blue", length=300)
blue_slider.pack()

# create a box to display the selected color
color_box = tk.Label(root, width=20, height=5, bg="#000000")
color_box.pack()

# create the "Select Color" button
select_button = tk.Button(root, text="Select Color", command=select_color)
select_button.pack()

# start the main loop
root.mainloop()
