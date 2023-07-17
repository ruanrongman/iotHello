import paho.mqtt.client as mqtt

def on_message(client, userdata, message):
    if message.retain == 1:
        # 检测到重复的消息
        print("Received a duplicate message: " + str(message.payload))
    else:
        # 处理新消息
        print("Received a new message: " + str(message.payload))

client = mqtt.Client()
client.on_message = on_message
client.connect("localhost", 1883, 60)
client.username_pw_set("cwl","1234")
client.subscribe("my/topic")

client.loop_forever()