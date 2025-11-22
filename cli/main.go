package main

import (
	"bufio"
	"fmt"
	"os"
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
		reader := bufio.NewReader(os.Stdin)
		str, _ := reader.ReadString('\n')

		var command = ""
		var args []string
		var lPtr = 0
		var rPtr = 0
		for ; rPtr < len(str); rPtr++ {
			fmt.Printf("str[i]: %c\n", str[rPtr])
			if str[rPtr] == ' ' || str[rPtr] == '\n' {
				if lPtr == 0 {
					command = str[lPtr:rPtr]
				} else {
					args = append(args, str[lPtr:len(str)-1])
				}

				lPtr = rPtr + 1
			}
		}

		var expectedArgs = argMap[command]
		if expectedArgs != len(args) {
			fmt.Printf("Invalid arguments.")
		} else {
			var commandHandler = cmdMap[command]
			commandHandler(args)
		}
	}
}
