# BeardServer
C++ Server for windows

This server is meant for a C# client (Unity) and so there are a few shared items.

	NetworkActions.cs
	ResponseCodes.cs

Packets sent to this server should be of the format:

	4 bytes			-> Total Packet size
	4 bytes			-> Packet ID
	4 bytes			-> packet data size
	remaining bytes	-> packet data
	
Packet data is a json object which contains the following keys:

	shared::NetworkKeys::kAction -> The value is a string which contains the action for the server to take. This is required
	shared::NetworkKeys::kData -> The value can be any data that will be passed to the server based on the action. This is not required
	
Server actions can be registered using TransmissionHandler::RegisterTransmissionHandler
Functions passed to this call must have the following:

	Client&                 -> ref to the calling client
	const std::string&      -> action that the client is calling
	unsigned int            -> transmission id
	const nlohmann::json&   -> transmission data
	
This registration is static, so it can be called from any class.
Client can be subclassed and if it is, Client::HandleTransmission should be overwritten. When done this way, a custom TransmissionHandler should be made. TransmissionHandler is a static class however, but the premise is the same for each one. Trying to create a templated version of TransmissionHandler resulted in a template hell.