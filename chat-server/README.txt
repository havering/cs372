Instructions for chat server:

Compile the server with the following command:

g++ chatserve.cpp -o chat

Server cannot be run without specifying port number as second argument. Example use:

./chat 30020

Program has no output until users connect, at which point connection, IP address, and socket number is announced, followed by initial message sent by client.

------------------------------

Instructions for chat client:

Python script requires three arguments in order to run: file name, host name, and port.

Example use:

python chatclient.py flip.engr.oregonstate.edu 30020

Server must be running in order for client to function. Client must elect to connect to numbered flip server that the chat server is running on (i.e. flip3) in order for number flip selection to work. 

-------------------------------

These files were tested on eos-class, flip1, and flip3. Connecting via client to flip.engr.oregonstate.edu will default connect to whatever flip server is available. 

Messages over 500 characters are not allowed. 

Sending a message that includes \quit will quit the chat room.

-------------------------------
