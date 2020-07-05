/* -------------------------------------------------------------------------- */
/* socket_channel.c                                                           */
/* -------------------------------------------------------------------------- */
int readSocketChnnl(int iClintFp, char *sVel);
int writeSocketChnnl(int iClientFp, double dVel);
int writeEndSocketChnnl(int iClientFp);
int closeClientSocket(int iClientFp);
