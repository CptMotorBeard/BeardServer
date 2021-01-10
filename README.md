# BeardServer
C++ Server for windows

Packets sent to this server should be of the format:

	4 bytes			-> Total Packet size
	4 bytes			-> Packet ID
	4 bytes			-> packet data size
	remaining bytes	-> packet data
	
Packet data is a json object which contains the following keys:

	act_id -> The value is a string which contains the action for the server to take
	dta_id -> The value can be any data that will be passed to the server based on the action
	
Server actions can be registered using TransmissionHandler::RegisterTransmissionHandler
Functions passed to this call must have the following:

	Client&                 -> ref to the calling client
	const std::string&      -> action that the client is calling
	unsigned int            -> transmission id
	const nlohmann::json&   -> transmission data
	
This registration is static, so it can be called from any class.