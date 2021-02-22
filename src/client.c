#include <client.h>

float own_position[3] = {0.0f, 0.0f, 0.0f};
float friend_position[3] = {0.0f,0.0f,0.0f};

// function definitions.
bool ConnectToHost(int PortNo, char *IPAddressBuffer);
void CloseConnection();
bool HostServer(int PortNo);
void Chat();
void *WaitForUserInput(); // Function executed on second thread when in chat to avoid blocking issues.
void ClearInputBuffer();
void GetValidPortNo(int *PortNo);
void GetValidIP(char *IPAddressBuffer);

// Global variables
bool bHasMessageWaiting;
bool bPerformExit;

char ReceiveBuffer[300]; // Used to hold messages sent by other party.
char InputBuffer[300]; // Used for any user input strings.

fd_set FDS; // file descriptor set used by winsock / sockets
struct timeval TV;
SOCKET ServerSocket; // SOCKET handle used to host server or connect to server.

enum { CLIENT, SERVER, UNSET } ConnectionMode = UNSET; // Used to set the program in host or client mode.

void* start_client(void* args) {

    //Zero strings
    memset(ReceiveBuffer,'\0',300);
    memset(InputBuffer,'\0',300);


	bool bConnectionSuccess= false;

    char InputChar = 0; // Used to store input from user temporarily if user input needs to be validated.
    char IPAddressBuffer[50] = "";
    int PortNo = 0;

    // Set timeval used by select function for wait period to 500 microseconds.
	TV.tv_sec = 0;
	TV.tv_usec = 500;

    #ifndef _WIN32
    setbuf(stdout, NULL); // Set stdout to flush straight away on POSIX platforms as opposed to waiting for newline.
    #endif // _WIN32

	while (ConnectionMode != CLIENT && ConnectionMode != SERVER) { // Loop until a valid ConnectionMode has been selected.
		printf("Press 1 to run chat server or 2 to run chat client and then press enter: ");
		InputChar = getchar();
        ClearInputBuffer(); // clear input buffer of newline

		if (InputChar == '1') { //SERVER MODE
			printf("\nYou have selected to run the chat server");
            ConnectionMode = SERVER;

            GetValidPortNo(&PortNo); // Ask user for Port No and make sure it's valid

            bConnectionSuccess = HostServer(PortNo);

            if (bConnectionSuccess) {
                printf("\nAccepted Connection!!\n");
                Chat();
            }
            else {
                printf("\n Connection failed! :( \n ");
                #ifdef _WIN32
                printf("\nError code: %d\n", WSAGetLastError());
                #endif // _WIN32
            }
            CloseConnection();
		}
		else if (InputChar == '2') { // CLIENT MODE
			printf("\nYou have selected to run the chat client.\n");
            ConnectionMode = CLIENT;

            GetValidPortNo(&PortNo); // Ask user for Port No and validate

            printf("\nYou have entered port no: %d\n", PortNo);

            GetValidIP(IPAddressBuffer); // Ask user for IP & validate.

            printf("\nYou have entered IP Address: %s\n", IPAddressBuffer);

            // Attempt to connect to socket
            bConnectionSuccess = ConnectToHost(PortNo, IPAddressBuffer);

            if (bConnectionSuccess) {
                printf("\nConnection Success!! \n");
                Chat();
            }
            else {
                printf("\nConnection failed!! :( \n");
                #ifdef _WIN32
                printf("\nError code: %d\n", WSAGetLastError());
                #endif // _WIN32
            }
            CloseConnection();
		}
		else {
			printf("\nYou have provided invalid input... try again!\n");
			ClearInputBuffer(); // Clear stdin
		}
	}

	printf("\nPress Enter to exit.");
	getchar();
	return 0;
}


bool ConnectToHost(int PortNo, char* IPAddressBuffer) {
    #ifdef _WIN32
    WSADATA wsadata; //Create wsadata struct used to start up WINSOCK

    int error = WSAStartup(0x0202, &wsadata); // Start up WINSOCK

    if (error)
        return false;

    if (wsadata.wVersion != 0x0202) // Did we get the right WINSOCK version?
    {
        WSACleanup();
        return false;
    }

    SOCKADDR_IN target;  //Fill out info required to connect to a socket on win32
    #else

    struct sockaddr_in target; //declare new internet socket address

    #endif // _WIN32

    target.sin_family = AF_INET; // IPv4 socket
    target.sin_port = htons (PortNo); // Port number of server to connect to w/ correct byte order
    target.sin_addr.s_addr = inet_addr(IPAddressBuffer); // provide pointer to IP address

    ServerSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //create socket
    if (ServerSocket == INVALID_SOCKET) {
        printf("\nERROR: UNABLE TO CREATE SOCKET\n");
        return false; // Couldn't create socket
    }

    // Try to actually connect to host. Moment of truth.
    #ifdef _WIN32
    if (connect(ServerSocket, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
    #else
    if (connect(ServerSocket,(struct sockaddr *) &target, sizeof(target)) == SOCKET_ERROR)
    #endif //
    {
        printf("\nERROR: SOCKET ERROR DURING CONNECT!\n");
        return false;
    }
    else
        return true;  // SUCCESS MOTHERFUCKER!!
}

bool HostServer(int PortNo)
{
    #ifdef _WIN32
    WSADATA wsadata;

    int error = WSAStartup(0x0202, &wsadata);

    if (error)
        return false;

    if (wsadata.wVersion != 0x0202) // Exit if it's not the right winsock version.
    {
        WSACleanup();
        return false;
    }
    #endif // _WIN32

    struct sockaddr_in ServerSockAddr;

    ServerSockAddr.sin_family = AF_INET; // IPv4 Address
    ServerSockAddr.sin_port = htons(PortNo); // Select Port No used for the socket

    //Accept a connection on any interface

    ServerSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // define TCP socket stream
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ServerSocket == INVALID_SOCKET)
        return false;

    if (bind(ServerSocket, (struct sockaddr *)&ServerSockAddr, sizeof(ServerSockAddr)) == SOCKET_ERROR)
    {
        return false;
    }

    listen(ServerSocket, SOMAXCONN);

    printf("\nSocket listening on port %d.  Waiting on connection from client...", PortNo);

    ServerSocket = accept(ServerSocket,NULL,NULL);

    return true;
}

void CloseConnection()
{
    if (ServerSocket)
        close(ServerSocket);

    #ifdef _WIN32
    WSACleanup(); //Clean up winsock
    #endif
}


// In chat
void Chat()
{
    bPerformExit = false;

    int SelectResponse; // Used to check if we have data waiting in the socket ready to be consumed.
    bHasMessageWaiting = false;
    pthread_t InputThread;

    printf("Connected.  Type your message and press enter to send it.  Type QUIT and press enter to Quit.\n");

    // Create the input thread before the chat loop
    if (pthread_create(&InputThread, NULL, WaitForUserInput, &bHasMessageWaiting))
    {
        printf("Error creating thread\n");
    }

    // Begin chat loop.  Allow it to continue until someone types QUIT or other party disconnects.
    do {
             if (bHasMessageWaiting) { // If the input thread has completed.
                pthread_join(InputThread, NULL);

                if (send(ServerSocket, InputBuffer, (int)strlen(InputBuffer), 0) == SOCKET_ERROR) {  // Send the message & check for errors.
                    #ifdef _WIN32
                    int SocketError = WSAGetLastError();
                    if (SocketError == WSAECONNRESET)
                        printf("Other party disconnected!\n");
                    else
                        printf("Socket Error! Code: %d\n", SocketError);
                    #endif // _WIN32
                    bPerformExit = true;
                }

                // Reset mesage waiting bool & InputBuffer
                bHasMessageWaiting = false;
                memset(InputBuffer,'\0',300);

                // Create the input thread again
                if (pthread_create(&InputThread, NULL, WaitForUserInput, NULL)) {
                    printf("Error creating thread\n");
                }
            }

            client_send_position();

            // Prep file descriptor set
            FD_ZERO(&FDS);
            FD_SET(ServerSocket, &FDS);

            SelectResponse = select(32, &FDS, NULL, NULL, &TV);

            if (SelectResponse == -1) {
                #ifdef _WIN32
                int SocketError = WSAGetLastError();
                if (SocketError == WSAECONNRESET)
                    printf("Other party disconnected!\n");
                else
                    printf("Socket Error! Code: %d\n", SocketError);
                #endif // _WIN32
                bPerformExit = true;
            }
            else if (SelectResponse > 0) {
                switch (recv(ServerSocket, &friend_position, 3*sizeof(float), 0)) {
                case SOCKET_ERROR: ; // Semi-colon used as empty statement for C stndard compliance
                    #ifdef _WIN32
                    int SocketError = WSAGetLastError();
                    if (SocketError == WSAECONNRESET)
                        printf("Other party disconnected!\n");
                    else
                        printf("Socket Error! Code: %d\n", SocketError);
                    #endif
                    bPerformExit = true;
                    break;

                case 0: //0 bytes received means other party gracefully closed the connection.
                    printf("Other party quit!\n");
                    bPerformExit = true;
                    break;

                default:
                    memset(ReceiveBuffer,'\0',strlen(ReceiveBuffer)); // Clear the chat buffer
                }
            }

    } while (bPerformExit != true);
}

void *WaitForUserInput() {
    // printf("bHasMessageWaiting is at address: %p\n", (void*)&bHasMessageWaiting);
    fgets(InputBuffer, 300, stdin);

    //trim newline characters so they aren't sent to the other party.

    size_t LastChar = strlen (InputBuffer) - 1;
    if (InputBuffer[LastChar] == '\n')
        InputBuffer[LastChar] = '\0';

    LastChar = strlen (InputBuffer) - 1;
    if (InputBuffer[LastChar] == '\r')
        InputBuffer[LastChar] = '\0';

    bHasMessageWaiting = true;
    pthread_exit(NULL);
    return NULL; // avoid warning regarding reaching end of non-void function
}

void ClearInputBuffer() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void GetValidPortNo(int *PortNo) {
    bool bValidInput = false;

    do {
        if (ConnectionMode == CLIENT)
            printf("\nType the port number (1-65535) you want to connect to on the server and press enter:");
        if (ConnectionMode == SERVER)
            printf("\nType the port number (1-65535) you want your server to listen on and press enter:");

        fgets(InputBuffer, 300, stdin);
        if ( sscanf(InputBuffer, "%d", PortNo) == 1 && *PortNo < 65535 && *PortNo > 0) {
            bValidInput = true;
        }
        else {
            printf("\nInvalid Input.  try again.\n");
            bValidInput = false;
        }
    } while (!bValidInput);
}

void GetValidIP(char *IPAddressBuffer) {
    bool bValidInput = false;
    char LastIPOctet[20] = ""; // Used as part of IP validation routine.
    int IPBufferLength = 0;
    char InputChar = 0;
    int Counter = 0;
    int OctetCounter = 0; // Used to ensure IP address provided by used has 4 octets / 3 dots.

   // Get IP address and validate
    do {
        bValidInput = false; // Reset bValidInput flag
        printf("\nWhat's the IP address you'd like to connect to? ");

        fgets(InputBuffer, 300, stdin); // Get IP from user and store in InputBuffer

        if (sscanf(InputBuffer, "%s", IPAddressBuffer) == 1) { // Store the first word provided by the user into the IPAddressBuffer string.

            bValidInput = true; // Valid thus far.  May be deemed invalid with next checks
            OctetCounter = 0;

            IPBufferLength = strlen(IPAddressBuffer);

            memset(LastIPOctet, '\0', 20); // Reinit this string in case this isn't the users first go at this.

            for (Counter = 0; Counter < IPBufferLength; Counter++) { // Test for a valid IP
                InputChar = IPAddressBuffer[Counter];

                if ((!isdigit(InputChar)) && (InputChar != '.')) { // First check if the character is valid in an IP address.
                    bValidInput = false;
                    break;
                }

                if (InputChar == '.' && strlen(LastIPOctet) == 0) { // Check if the user is ending an octet prematurely.
                    bValidInput = false;
                    break;
                }

                if (InputChar == '.') { // Count how many octets / dots are listed in IP.
                    OctetCounter++;
                }

                if (InputChar == '.' && strlen(LastIPOctet) > 0) { // Check if the last octet input was valid
                    if (atoi(LastIPOctet) > 255 || atoi(LastIPOctet) < 0) {
                        bValidInput = false;
                        break;
                    }
                    memset(LastIPOctet, '\0', 20); // Reset this string so it can be used for the next octet
                }
                else {
                    char cToStr[2]; // Convert character to String so strcat can be used.
                    cToStr[0] = InputChar;
                    cToStr[1] = '\0';
                    strcat(LastIPOctet, cToStr); // Add current character onto LastIPOctet string used by IP validation routine.
                }
            }
            if (OctetCounter != 3) // Invalid if the user has not provided 4 octets.
                bValidInput = false;
            if (atoi(LastIPOctet) > 255 || atoi(LastIPOctet) < 0) // Check last octet
                bValidInput = false;
        }
        if (!bValidInput)
            printf("\nInvalid Input IP address.  Try again.\n");
    }
    while (!bValidInput);
}

void client_send_position()
{
    send(ServerSocket, &own_position, 3*sizeof(float), 0);
}
