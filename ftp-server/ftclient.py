# Diana O'Haver
# Program 2
# Python FTP client
# Base solution sourced from: http://www.bogotobogo.com/python/python_network_programming_tcp_server_client_chat_server_chat_client_select.php

import sys
import socket
import select

# function to handle creating contact with the server
def initiate_contact(s, host, port):
    # connect to remote host
    try :
        s.connect((host, port))
    except :
        print 'Unable to connect'
        sys.exit()
    return s

# function to send the message
def send_message(s, msg):
    s.send(msg)			
    return

# writing to a file in python: http://www.tutorialspoint.com/python/python_files_io.htm
# creating a file if it doesn't exist: http://stackoverflow.com/questions/2967194/open-in-python-does-not-create-a-file-if-it-doesnt-exist
def receive_file(sock, filename):
    # incoming message from remote server, s
    data = sock.recv(4096)		# 4096 is the buffer size
    if not data :
        print '\nDisconnected from chat server'
        sys.exit()
    else :
        # first check if the error message was received
		# if so, don't open the file and just output the error message
        if data == 'ERROR: File does not exist':
            print data
        else:
            # open the file 
            newfile = open(filename, "w+")
            newfile.write(data)
            print 'transfer complete'
    return

# function to handle the case where the directory is just supposed to be output to the screen
def receive_dir(sock):
    # incoming message from remote server, s
    data = sock.recv(4096)		# 4096 is the buffer size
    if not data :
        print '\nDisconnected from chat server'
        sys.exit()
    else :
        #print data
        sys.stdout.write(data); sys.stdout.flush()     
    return

# main function for handling input validation and calls to other functions
def ftp_client():
    if(len(sys.argv) < 5):			# must have 4 arguments to operate program
        print 'Minimum usage : python ftclient.py hostname port action receiveport'
        print 'Additional usage:'
        print 'Allowed actions: -l (list) or -g (get)'
        print 'With -g, specify filename in format: -g filename port'
        sys.exit()

    host = sys.argv[1]				# second argument
    port = int(sys.argv[2])			# third argument
	
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5000)	# timeout of 2 seconds from original source times out way too quickly

    # only valid usage has -l and -g as the action command
    if ((sys.argv[3] != '-l') and (sys.argv[3] != '-g')):
        print 'Allowed actions: -l (list) or -g (get)'
        print 'You entered ' + sys.argv[3]
        sys.exit()
    else:
        action = sys.argv[3]
        # determine if the user wants to get a file or just list dir contents
        if (sys.argv[3] == '-l'):
            recvport = sys.argv[4]
        # if not list, then need to assign file name variable
        else:
            filename = sys.argv[4]
            recvport = sys.argv[5]
     
		
    initiate_contact(s, host, port)

    print 'Connected to remote host. Sending request.'
     
    # send off the request
    if (len(sys.argv) == 5):
        msg = action + " " + recvport
    else:
        msg = action + " " + filename + " " + recvport
				
    send_message(s, msg)
    
    # response depends on type of message sent
    if (action == '-l'):
        receive_dir(s)
    else:
        receive_file(s, filename)

if __name__ == "__main__":

    sys.exit(ftp_client())