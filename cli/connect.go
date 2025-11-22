package main

type ConnectService struct {
	tcpService *TcpService
}

func (cs *ConnectService) Execute(args []string) {
	var address = args[0]
	cs.tcpService.ConnectTcp(address)
}

func (cs *ConnectService) GetArgCount() int {
	return 1
}
