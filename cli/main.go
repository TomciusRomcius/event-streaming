package main

import (
	"bufio"
	"fmt"
	"os"
)

var cmdMap = make(map[string]CmdService)

func socketListener(tcpService *TcpService) {
	for {
		var msg, err = tcpService.ReceiveMessageStr()
		if err != nil {
			tcpService.connection.Close()
			fmt.Println("Client disconnected. Connect again using connect command")
			break
		}

		fmt.Printf("Incoming message: %s\n", msg)
	}
}

func main() {
	var tcpService = TcpService{}
	tcpService.onConnect = func() {
		go socketListener(&tcpService)
	}

	var connectService = ConnectService{tcpService: &tcpService}
	cmdMap["connect"] = &connectService

	var subscribeService = SubscribeService{tcpService: &tcpService}
	cmdMap["subscribe"] = &subscribeService

	var createEventTypeService = CreateEventTypeService{
		tcpService: &tcpService,
	}
	cmdMap["create-event-type"] = &createEventTypeService

	var produceEventService = ProduceEventService{
		tcpService: &tcpService,
	}
	cmdMap["produce-event"] = &produceEventService

	commandListener()
}

func commandListener() {
	for {
		reader := bufio.NewReader(os.Stdin)
		str, _ := reader.ReadString('\n')

		var command = ""
		var args []string
		var lPtr = 0
		var rPtr = 0
		for ; rPtr < len(str); rPtr++ {
			if str[rPtr] == ' ' || str[rPtr] == '\n' {
				if lPtr == 0 {
					command = str[lPtr:rPtr]
				} else {
					args = append(args, str[lPtr:rPtr])
				}

				lPtr = rPtr + 1
			}
		}

		var commandHandler = cmdMap[command]
		if commandHandler == nil {
			fmt.Println("Invalid command")
		} else {
			var expectedArgCount = commandHandler.GetArgCount()
			if len(args) != expectedArgCount && expectedArgCount != -1 {
				fmt.Printf("Unexpected argument count.\n")
			} else {
				commandHandler.Execute(args)
			}
		}
	}
}
