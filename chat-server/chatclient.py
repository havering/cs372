# Diana O'Haver
# Program 1
# Python chat client
# Base solution sourced from: http://www.bogotobogo.com/python/python_network_programming_tcp_server_client_chat_server_chat_client_select.php

import sys
import socket
import select
 
#### Farm out error checking for cleanliness ####
def check_name():
    # get the user's handle
    user_handle = raw_input("What is your name? ")
	
	# error handling if user enters handle that is too large
    while (len(user_handle) > 10): 
        print "Error! Your handle must be 10 characters or less."
        user_handle = raw_input("Enter your name again: ")
	
    return user_handle

def initiate_contact(s, host, port):
    # connect to remote host
    try :
        s.connect((host, port))
        # send initial message if successful
        connect_msg = "Thanks for connecting me!"
        s.send(connect_msg)
    except :
        print 'Unable to connect'
        sys.exit()
    return

def send_message(s, msg, name):
    s.send('\n[' + name + ']: ' + msg)			# this line allows the user's handle to be sent to the other side
    return

def receive_message(sock, name):
    # incoming message from remote server, s
    data = sock.recv(4096)		# 4096 is the buffer size
    if not data :
        print '\nDisconnected from chat server'
        sys.exit()
    else :
        #print data
        sys.stdout.write(data)
        sys.stdout.write('[' + name + ']: '); sys.stdout.flush()     
    return

def chat_client():
    if(len(sys.argv) < 3) :			# must have 3 arguments to operate program
        print 'Usage : python chatclient.py hostname port'
        sys.exit()

    host = sys.argv[1]				# second argument
    port = int(sys.argv[2])			# third argument
     
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(2)

    name = check_name()		
		
    initiate_contact(s, host, port)
     
    print 'Connected to remote host. You can start sending messages'
    sys.stdout.write('[' + name + ']: '); sys.stdout.flush()
     
    while 1:
        socket_list = [sys.stdin, s]
         
        # Get the list sockets which are readable
        ready_to_read,ready_to_write,in_error = select.select(socket_list , [], [])
         # loop through the ready to read sockets looking for a match to s, defined above
        for sock in ready_to_read:             
            if sock == s:
                receive_message(sock, name)
            
            else :
                # user entered a message
                msg = sys.stdin.readline()
				# messages must be 500 characters or less
                if (len(msg) > 500):
                    print '\nError: messages must be 500 characters or less'
                    sys.stdout.write('[' + name + ']: '); sys.stdout.flush()
                # check if message is \quit - if so, exit
                elif '\quit' in msg:
                    print '\nThanks for chatting!'
                    sys.exit()
                else:
                    send_message(s, msg, name)
                    sys.stdout.write('[' + name + ']: '); sys.stdout.flush() 

if __name__ == "__main__":

    sys.exit(chat_client())