package main

import (
	"encoding/json"
	"fmt"
	"strconv"
)

type propDeclarationType struct {
	Name     string `json:"key"`
	Datatype int    `json:"type"`
}

type createEventMessageType struct {
	MsgType   string                `json:"type"`
	EventType string                `json:"eventType"`
	Props     []propDeclarationType `json:"properties"`
}

type CreateEventTypeService struct {
	tcpService *TcpService
}

func (service CreateEventTypeService) Execute(args []string) {
	var remainingArgs = args[1:]

	var argMap = make(map[string][]string, 0)
	for i := range remainingArgs {
		var arg = remainingArgs[i]
		if arg[0] == '-' && arg[1] == '-' {
			var equalIndex = 0
			var lPtr = 2
			for ; lPtr < len(arg); lPtr++ {
				if arg[lPtr] == '=' {
					equalIndex = lPtr
					break
				}
			}
			var key = arg[2:equalIndex]
			var value = arg[equalIndex+1:]
			argMap[key] = append(argMap[key], value)
		}
	}

	var msg = createEventMessageType{
		MsgType:   "create-event-type",
		EventType: args[0],
	}

	for i := range argMap["propName"] {
		var propName = argMap["propName"][i]
		var datatype = argMap["datatype"][i]
		var nDataType, _ = strconv.Atoi(datatype)
		msg.Props = append(msg.Props, propDeclarationType{Name: propName, Datatype: nDataType})
	}

	var msgJson, err = json.Marshal(msg)
	if err != nil {
		fmt.Println(err.Error())
	}
	service.tcpService.SendMessageBytes(msgJson)
	fmt.Println("Created an event type succesfully")
}

func (service *CreateEventTypeService) GetArgCount() int {
	return -1 // infinite args
}
