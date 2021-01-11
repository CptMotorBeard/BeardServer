namespace shared
{
#if !__cplusplus
    public partial class BeardServerManager
    {
        public
#endif
    enum ResponseCodes
        {
            OK = 200,
            NotFound = 404,
            GatewayTimeout = 504,

            Invalid = 1000,
            InvalidJson,
            InvalidPacket,

            DataNotFound = 2000,
            DataAlreadyExists,

            ClientDisconnected = 8000
        }
#if __cplusplus
    ;
#else
    }
#endif
}