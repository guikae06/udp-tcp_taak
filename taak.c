#ifdef _WIN32
    #define _WIN32_WINNT _WIN32_WINNT_WIN7
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <windows.h>
    #include <unistd.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netdb.h>
    #include <netinet/in.h> 
    #include <arpa/inet.h>
    #include<errno.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <string.h>
#endif
