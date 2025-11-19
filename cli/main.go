package main

import (
	"fmt"
	"time"
)

type cmdHandler func([]string)

var cmdMap = map[string]cmdHandler{
	"subscribe": HandleSubscribe,
}

var argMap = map[string]int{
	"subscribe": 1,
}

func main() {
	go commandListener()
	for {
		time.Sleep(time.Second)
	}
}

func commandListener() {
	for {
		var str = ""
		fmt.Scanf("%s", &str)

		var commandHandler = cmdMap[str]
		var argCount = argMap[str]

		var formatStr = "%s"
		for i := 0; i < argCount; i++ {
			formatStr += " %s"
		}

		var args []string
		readArgs, err := fmt.Sscanf(str, formatStr, &args)
		if err != nil {
			fmt.Printf("Error: %s\n", err.Error())
		}
		fmt.Printf("arg count: %d\n", readArgs)

		commandHandler(args[1 : len(args)-1])
	}
}
