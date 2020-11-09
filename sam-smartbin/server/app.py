from flask import Flask, request
import pyrebase


config = {
    'apiKey': "AIzaSyA50ZEbRRKLiy_qwa_vE462-J-_h2CT64k",
    'authDomain': "smart-bin-da279.firebaseapp.com",
    'databaseURL': "https://smart-bin-da279.firebaseio.com",
    'projectId': "smart-bin-da279",
    'storageBucket': "smart-bin-da279.appspot.com",
    'messagingSenderId': "265397120155",
    'appId': "1:265397120155:web:ec24ff87b20c9e5ec4aa3e",
    'measurementId': "G-VNW13T1RSQ"}

import pyrebase
fire_base = pyrebase.initialize_app(config)
auth = fire_base.auth()
database = fire_base.database()
def update_fire_base(child, bin_id, **new_data):
    rx = database.child(child).child(bin_id).set(new_data)
    print(rx)
   



app = Flask(__name__)


@app.route("/")
def hello():
    return "SMARTEST BIN"


@app.route("/level", methods=['POST'])
def sms_reply():
    """Respond to incoming calls with a simple text message."""
    # Fetch the message
    level = request.args.get('level')
    bin_id = request.args.get('bin_id')
    gps = request.args.get('gps')
    print("recived:", level, gps, "from", bin_id)
    update_fire_base("bins", bin_id, level=int(level), gps=gps)
    reply = "received!!!"
    # Create reply
    return str(reply)


def start_server():
    if __name__ == "__main__":
        app.run(debug=True)

# #######################################################################################################

start_server()

