# A Qt-based Trip Planner
> It translates messages through TCP
---

**Features**
1. The client send queries to the server for trip planings.
2. The server analysises queries and sends planings back to the client.

**Notes**
- After QT5, the function *incomingConnection* changes from *QTcpServer::incomingConnection(int socketId)* to *QTcpServer::incomingConnection(qintptr socketId)*.
