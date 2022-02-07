from ipaddress import IPv4Address
from web import create_app

# Change the ip address below with yours by using ipconfig
HOST = '192.168.1.4'
PORT = '9000'

app = create_app()

if __name__ == '__main__':
    app.run(host=HOST, port=PORT, debug=True)
