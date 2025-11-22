package main

import (
	"encoding/binary"
	"fmt"
	"net"
)

type TcpService struct {
	connection net.Conn
}

func (cm *TcpService) ConnectTcp(hostname string) {
	conn, err := net.Dial("tcp", hostname)
	if err != nil {
		fmt.Printf("Error while connecting to %s: %s", hostname, err.Error())
	} else {
		cm.connection = conn
	}
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
