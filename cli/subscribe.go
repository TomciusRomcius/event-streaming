package main

import (
	"fmt"
	"strconv"
)

type SubscribeService struct {
	tcpService *TcpService
}

func (service *SubscribeService) Execute(args []string) {
	var eventType = args[0]
	groupId, err := strconv.Atoi(args[1])
	if err != nil {
		return
	}

	var msg = fmt.Sprintf(`
	{
		"type": "subscribe-to-event-type",
		"eventType": "%s",
		"groupId": %d
	}`, eventType, groupId)

	service.tcpService.SendMessage(msg)
	fmt.Println("Subscribed succesfully")
}

func (service *SubscribeService) GetArgCount() int {
	return 2
}
