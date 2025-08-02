#include <iostream>
#include "application/application.h"

int main()
{
	//auto streamer = EventStreamer();
	//std::map<std::string, PropertyType> properties = {
	//	{ "key1", PropertyType::INT },
	//	{ "key2", PropertyType::DECIMAL }
	//};

	//auto eventType = EventType("TestEvent", properties);

	//streamer.RegisterEventType(eventType);
	//streamer.Subscribe(eventType, "127.0.0.1");

	//std::map<std::string, void*> values = {
	//	{ "key1", (void*)new int(42) },
	//	{ "key2", (void*)new double(42.2) },
	//};
	//auto testEvent = Event(
	//	eventType,
	//	values
	//);
	//streamer.ProduceEvent(testEvent);
	std::cout << "Event streaming application started." << std::endl;

	Application app;
	app.Start();
	return 0;
}
