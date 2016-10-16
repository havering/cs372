Instructions for FTP server:

Compile the server with the following command:

gcc ftserver.c -o ftserver

Server cannot be run without specifying port number as second argument. Example use:

./ftserver 6000

Program will output confirmation of launch on specified port number. No further output will be provided until connection is made from a client. Confirmation will be output when directory list or valid file is sent. Perror will output error message if requested file does not exist. Server will run until manually shut down by user with Ctrl+C.

------------------------------

Instructions for chat client:

Python script requires 5 arguments in order to run: host name, host port, action, and receiving port

Example use:

python ftclient.py flip.engr.oregonstate.edu 6000 -l 6001

Server must be running in order for client to function. Client must elect to connect to numbered flip server that the chat server is running on (i.e. flip3) in order for numbered flip selection to work. Use of -l will output the directory contents of the server directory but will not write to a file. Use of -g on a valid file will result in output confirming connection to server and completion of transfer. Use 'cat filename.txt' in order to confirm contents of transferred file. Use of -g on an invalid file will result in output of error message and no file created.

-------------------------------

These files were tested on eos-class, flip1, and flip2. 

Data greater than 4095 characters is not permitted by either server or client.

-------------------------------
