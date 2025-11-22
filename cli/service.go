package main

type CmdService interface {
	Execute(args []string)
	GetArgCount() int
}
