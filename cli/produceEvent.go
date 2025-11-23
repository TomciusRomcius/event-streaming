package main

import (
	"encoding/json"
	"strconv"
)

type ProduceEventService struct {
	tcpService *TcpService
}

type propDefinitionType struct {
	Name  string `json:"key"`
	Value any    `json:"value"`
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
		var nValue, nErr = strconv.ParseFloat(value, 64)
		if nErr != nil {
			msg.Props = append(msg.Props, propDefinitionType{Name: name, Value: value})
		} else {
			msg.Props = append(msg.Props, propDefinitionType{Name: name, Value: nValue})
		}
	}

	var msgJson, _ = json.Marshal(msg)
	service.tcpService.SendMessageBytes(msgJson)
}

func (service *ProduceEventService) GetArgCount() int {
	return -1
}
