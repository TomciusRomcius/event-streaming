#pragma once

#ifdef WIN32
typedef SOCKET SocketType;
#endif
#ifdef __linux__
typedef int SocketType;
#endif
