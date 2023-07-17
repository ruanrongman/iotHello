import json
import time

import flask
from flask import Flask
from flask_mqtt import Mqtt
from flask_cors import CORS


app = Flask(__name__)
CORS(app)
app.config['MQTT_BROKER_URL'] = '106.14.145.57'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'cwl'  # 当你需要验证用户名和密码时，请设置该项
app.config['MQTT_PASSWORD'] = '19260817'  # 当你需要验证用户名和密码时，请设置该项
app.config['MQTT_KEEPALIVE'] = 5  # 设置心跳时间，单位为秒
app.config['MQTT_TLS_ENABLED'] = False  # 如果你的服务器支持 TLS，请设置为 True
topic = '/flask/mqtt'

mqtt_client = Mqtt(app)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("cwl/temperature/client")         # 订阅消息


def on_message(client, userdata, msg):
    global client_message
    print("主题:"+msg.topic+" 消息:"+str(msg.payload.decode('utf-8')))
    client_message=msg.payload.decode('utf-8')


def on_subscribe(client, userdata, mid, granted_qos):
    print("On Subscribed: qos = %d" % granted_qos)



def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection %s" % rc)
def mqtt_my():
    data = {
        "control": 'on',
        "timestamp": time.time(),

    }
    param = json.dumps(data)
    mqtt_client.publish("/fish", "0")
def mqtt_my_close():
    data = {
        "control": 'off',
        "timestamp": time.time(),

    }
    param = json.dumps(data)
    mqtt_client.publish("/fish", "1")
@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
   data = dict(
       topic=message.topic,
       payload=message.payload.decode()
  )
   print('Received message on topic: {topic} with payload: {payload}'.format(**data))
@app.route('/open',methods=['POST'])
def open():
    mqtt_my()
    return "{\"data\":\"open\"}"
@app.route("/close",methods=['POST'])
def close():
    mqtt_my_close()
    return "{\"data\":\"close\"}"
@app.route("/status",methods=['GET'])
def status():

    return "ok"
@app.route('/')
def hello_world():
    return flask.render_template("hello.html")


if __name__ == '__main__':

    app.run()
