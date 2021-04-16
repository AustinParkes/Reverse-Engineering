"""
Reconstruct Flash contents in order, from SPI CSV readout
"""
# Parse 24 bit address and store it as hex integer in list
def parse_address(comm):
	# First address byte
	line = csv_file.readline()			
	addr1 = line.split(',')[mosi_col]
	addr1 = addr1[2:]						# Gets rid of 0x
	# Second address byte
	line = csv_file.readline()			
	addr2 = line.split(',')[mosi_col]		
	addr2 = addr2[2:]						# Gets rid of 0x
	
	# Third address byte
	line = csv_file.readline()				
	addr3 = line.split(',')[mosi_col]
	addr3 = addr3[2:]						# Gets rid of 0x
	
	addr = addr1 + addr2 + addr3			# Concatenate string addresses
	hex_addr = int(addr, 16)                 # convert hex string to int
	return hex_addr
	
# Parse 32 bit data and store it as hex string
def parse_data(addr,start_pid):
	dummy = csv_file.readline()				# Skip dummy data
	addr_offset=0
	# Get bytes data from an address
	while(1):    
	                   
		# First byte
		line = csv_file.readline()	            # Strip a readline			
		data1 = line.split(',')[miso_col]       # Get data as a hex string
		data1 = data1[2:4]						# Get rid of 0x and newline
		#print(addr)
		memory[addr] = int(data1,16)            # Store byte into memory model
		print(line)
		# Second byte
		line = csv_file.readline()				
		data2 = line.split(',')[miso_col]
		data2 = data2[2:4]	
		memory[addr + 1] = int(data2,16)	
		print(line)			
		# Third byte
		line = csv_file.readline()				
		data3 = line.split(',')[miso_col]
		data3 = data3[2:4]		
		memory[addr + 2] = int(data3,16)
		print(line)				
		# Fourth byte
		line = csv_file.readline()				
		data4 = line.split(',')[miso_col]
		data4 = data4[2:4]						
		memory[addr + 3] = int(data4,16)
		print(line)
		# Check if pid has changed
		line = csv_file.readline()
		#print(line)
		pid = line.split(',')[pid_col]	
		print(line)
		
    	# Keep storing data until pid changes
		if (pid == start_pid):    
			addr = addr + 4                 # Next address in memory
			if (addr >= 2097152):            # Don't go beyond max index
				print(5)
				return -1                   # Exit
		else:
			return pid, line

	
# Possible columns we'll read
pid_col = 1
mosi_col = 2
miso_col = 3

# Only command we care about
fast_read = "0x0B"


# Construct a memory model before updating it
capacity = 2097152
memory = bytearray(capacity) # memory capacity of SPI is 2097152
index=0

# Populate memory with 0xFF
while (index < (capacity) ):
	memory[index]=0xff
	index = index + 1

csv_file = open("spi_analyzer_output.csv",'r')

"""
Go through each packet ID, get it's address and data,
and store it in a list. We skip anything that isn't a fast read.
"""

# Strip some variables from the first iteration
line = csv_file.readline() 			# First line is header information
line = csv_file.readline() 			# Strip a line from the loop
pid = line.split(',')[pid_col]		# Get packet id	

"""
we need to return the correct line
"""
prev_pid=1 		# Don't want this to equal pid at start of loop
pid=0
while (pid != -1):  			        # -1 is exit condition
	#print(pid)
	#print(prev_pid)
	if (pid != prev_pid):					# Don't read a command until pid changes
	
		command = line.split(',')[mosi_col] # Get command
		
		if (command == fast_read):
			print("pid: " + str(pid))
			print("comm: " + command)
			print("line: " + line)
			addr = parse_address(command)			# parse address from 'read' command
			print("addr: " + str(hex(addr)))
			prev_pid = pid
			pid, line = parse_data(addr, pid)        	# parse data from address(es) and store in memory model
			#print(line)
		else:                              # Read lines until pid changes
			prev_pid = pid		
			line = csv_file.readline()
			pid = line.split(',')[pid_col]		# Get packet id	

f = open("opdump.bin", 'wb')
f.write(memory[:])
f.close


#print(addresses)
#print((data_arr[0:20]))	


"""
Sort data in order by address, and fill missing addresses with 0xFF
"""
"""
sorted_addresses = []
sorted_data = []
addr_hit=0
index = 0
expected_address = 0x0
while (expected_address <= max(addresses)):
	# Order the data
	if (addresses[index] != expected_address):
		# Search for the index that our expected address is at, to get the corresponding data
		temp_index = index
		while (temp_index < len(addresses)):
			if (addresses[temp_index] == expected_address):
				addr_hit=1									# We found an address that matches expected!
				sorted_addresses.append(expected_address)	# sort address
				sorted_data.append(data_arr[temp_index])	# sort data
				break
			temp_index = temp_index + 1	
		# No address found, so fill with 0xFF		
		if (addr_hit != 1):
			sorted_addresses.append(expected_address)
			sorted_data.append(0xFF)


	# Keep data the same
	else:
		sorted_addresses.append(expected_address)
		sorted_data.append(data_arr[index])
		
	# Reset and increment indices		
	#print(expected_address)
	#if (expected_address == 0x28):
		#print(sorted_addresses)
		#print(sorted_data)
	addr_hit=0		
	index=index+1
	expected_address=expected_address+4	
		
	
	
	

print(sorted_addresses[0:10])
print(sorted_data[0:10])
print("Done")
"""	
