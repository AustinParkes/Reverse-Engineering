import socket
import struct

# Ensure all bytes are sent over socket.
def send_cmd(cmd, msg_len):
    totalsent = 0
    while totalsent < msg_len:
        sent = sock.send(cmd[totalsent:])
        if sent == 0:
            raise RuntimeError("socket connection broken")
        totalsent = totalsent + sent
    return totalsent
        
# Receive with a pre-defined message length
def receive(msg_len):
    chunks = []
    bytes_recv = 0
    while bytes_recv < msg_len:
        chunk = sock.recv(min(msg_len - bytes_recv, 2048))
        if chunk == b'':
            raise RuntimeError("socket connection broken")
        chunks.append(chunk)
        bytes_recv = bytes_recv + len(chunk)
    return b''.join(chunks)


# Execute a command (Little Endian Format)
def exec_cmd():
	cmd = b"ADMN"   						# Header
	cmd += b"\x06\x00\x00\x00"				# MSG Length
	cmd += b"\xF1\x00\x00\x00"				# MSG ID
	cmd += b"\x04\x04\x00\x00\x00\x00"		# MSG
	
	msg_len = len(cmd)
	send_cmd(cmd, msg_len)
	
	recv_msg = receive(34)
	print(recv_msg)

# Reboot machine (Little Endian Format)					
def reboot_cmd():	
	cmd = b"ADMN"   			# Header
	cmd += b"\x01\x00\x00\x00"	# MSG Length
	cmd += b"\xF2\x00\x00\x00"	# MSG ID
	cmd += b"\x10"				# MSG
	
	# Send a fixed length message
	msg_len = len(cmd)
	send_cmd(cmd, msg_len)	
	
	# Doesn't receive a message upon success.
	
# Reflect (Little Endian Format)	
def reflect_cmd():
	cmd = b"ADMN"   					# Header
	cmd += b"\x06\x00\x00\x00"			# MSG Length (must 6 or greater, but every byte beyond 6 is ignored)
	cmd += b"\xF3\x00\x00\x00"			# MSG ID 	 (Can be anything)
	cmd += b"\x08\x08\x00\x00\x00\x00"	# MSG (Little Endian)

	# Send a fixed length message
	msg_len = len(cmd)
	send_cmd(cmd, msg_len)
	
	# Receive a fixed length message back
	recv_msg = receive(18)
	print(recv_msg)

sock = socket.socket()

# binary-admin socket connection over TCP
sock.connect(('169.254.15.2', 31338))

# Timeout of 'x' seconds for blocking socket operations
# sock.settimeout(5)   
	
exec_cmd()
#reboot_cmd()
#reflect_cmd()
		
# Don't forget to close the session	
sock.close()



