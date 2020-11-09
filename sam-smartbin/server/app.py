from flask import Flask, request

'''
import pyrebase
fire_base = pyrebase.initialize_app(config)
auth = fire_base.auth()
database = fire_base.database()

def update_fire_base(hub_table, row_id, **new_data):
   
'''


app = Flask(__name__)


@app.route("/")
def hello():
    return "SMARTEST BIN"


@app.route("/level", methods=['POST'])
def sms_reply():
    """Respond to incoming calls with a simple text message."""
    # Fetch the message
    command = request.args.get('level')
    print("recived:", command)
    reply = "received!!!"
    # Create reply
    return str(reply)


def start_whatspp_twilio():
    if __name__ == "__main__":
        app.run(debug=True)

# #######################################################################################################

start_whatspp_twilio()
