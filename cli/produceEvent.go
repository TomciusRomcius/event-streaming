package main

import "encoding/json"

type ProduceEventService struct {
	tcpService *TcpService
}

type propDefinitionType struct {
	Name  string `json:"key"`
	Value any    `json:"type"`
}

type produceEventMsgType struct {
	MsgType   string               `json:"type"`
	EventType string               `json:"eventType"`
	Props     []propDefinitionType `json:"properties"`
}

func (service *ProduceEventService) Execute(args []string) {
	var eventType = args[0]
	var argMap = ParseNamedArguments(args[1:])
	var msg = produceEventMsgType{
		MsgType:   "produce-event",
		EventType: eventType,
	}

	for i := range argMap["propName"] {
		var name = argMap["propName"][i]
		var value = argMap["value"][i]
		msg.Props = append(msg.Props, propDefinitionType{Name: name, Value: value})
	}

	var msgJson, _ = json.Marshal(msg)
	service.tcpService.SendMessageBytes(msgJson)
}

func (service *ProduceEventService) GetArgCount() int {
	return -1
}
