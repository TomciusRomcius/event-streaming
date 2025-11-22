package main

type SubscribeService struct {
	tcpService *TcpService
}

func (service *SubscribeService) Execute(args []string) {
}

func (service *SubscribeService) GetArgCount() int {
	return 1
}
