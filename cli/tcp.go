package main

import (
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

func SendMessageTcp(buffer []byte, connectionManager *TcpService) {

}
