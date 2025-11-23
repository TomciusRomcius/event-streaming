package main

import (
	"encoding/binary"
	"fmt"
	"net"
)

type TcpService struct {
	connection net.Conn
	onConnect  func()
}

func (cm *TcpService) ConnectTcp(hostname string) {
	conn, err := net.Dial("tcp", hostname)
	if err != nil {
		fmt.Printf("Error while connecting to %s: %s", hostname, err.Error())
	} else {
		cm.connection = conn
		cm.onConnect()
	}
}

// blocking call without select()
func (cm *TcpService) ReceiveMessageStr() (string, error) {
	var sizeBuffer []byte = make([]byte, 4)
	var _, err = cm.connection.Read(sizeBuffer)
	if err != nil {
		return "", err
	}

	var msgSize = binary.BigEndian.Uint32(sizeBuffer)
	var msgBuffer = make([]byte, msgSize)
	var _, _ = cm.connection.Read(msgBuffer)

	return string(msgBuffer), nil
}

func (cm *TcpService) SendMessage(message string) {
	var messageBuf = []byte(message)
	var bufferSize = len(messageBuf)
	var socketBuffer []byte = make([]byte, bufferSize+4)
	var sizeBuffer []byte = make([]byte, 4)
	binary.BigEndian.PutUint32(sizeBuffer, uint32(bufferSize))
	copy(socketBuffer[:4], sizeBuffer)
	copy(socketBuffer[4:], messageBuf)
	cm.connection.Write(socketBuffer)
}

func (cm *TcpService) SendMessageBytes(message []byte) {
	var bufferSize = len(message)
	var socketBuffer []byte = make([]byte, bufferSize+4)
	var sizeBuffer []byte = make([]byte, 4)
	binary.BigEndian.PutUint32(sizeBuffer, uint32(bufferSize))
	copy(socketBuffer[:4], sizeBuffer)
	copy(socketBuffer[4:], message)
	cm.connection.Write(socketBuffer)
}
