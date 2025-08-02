// #include <map>
// #include <string>
// enum TcpRequestType
// {
//     CREATE_EVENT_TYPE = 1,
//     SUBSCRIBE_TO_EVENT = 2,
//     PRODUCE_EVENT = 3,
//     UNSUBSCRIBE_FROM_EVENT = 4,
// };

// struct ITcpRequest
// {
//     virtual TcpRequestType GetRequestType() const = 0;
// };

// struct CreateEventTypeRequest : public ITcpRequest
// {
//     std::string eventTypeName;
//     std::map<std::string, std::string> properties;

//     TcpRequestType GetRequestType() const override
//     {
//         return CREATE_EVENT_TYPE;
//     }
// };

// struct SubscribeToEventRequest : public ITcpRequest
// {
//     std::string eventTypeName;

//     TcpRequestType GetRequestType() const override
//     {
//         return SUBSCRIBE_TO_EVENT;
//     }
// };