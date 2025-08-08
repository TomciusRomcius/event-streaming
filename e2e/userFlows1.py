import socket
import time

host = "127.0.0.1"
port = 9000

createEventTypeReq = """
{
    "type": "create-event-type",
    "eventType": "test-event",
    "properties": [
        {
            "key": "Key1",
            "type": 0
        },
        {
            "key": "Key2",
            "type": 1
        }
    ] 
}
"""

subscribeToEventTypeReq = """
{
    "type": "subscribe-to-event-type",
    "eventType": "test-event"
}
"""

produceEventReq = """
{
    "type": "produce-event",
    "eventType": "test-event",
    "properties": [
        {
            "key": "Key1",
            "value": "Cool value"
        },
        {
            "key": "Key2",
            "value": 2.99
        }
    ] 
}
"""

def generate_tcp_message(message: str):
    messageBytes = message.encode(encoding="utf-8")
    bufSize = len(messageBytes)
    return bufSize.to_bytes(4, 'big') + messageBytes

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((host, port))
    s.send(generate_tcp_message(createEventTypeReq))
    s.send(generate_tcp_message(subscribeToEventTypeReq))
    s.send(generate_tcp_message(produceEventReq))
    print("listening")
    receivedBytes = s.recv(1024)
    msg = receivedBytes.decode()
    print(msg)