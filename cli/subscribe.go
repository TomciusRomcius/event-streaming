package main

import "fmt"

func HandleSubscribe(args []string) {
	for i := range args {
		fmt.Printf("Arg: %s\n", args[i])
	}
}
