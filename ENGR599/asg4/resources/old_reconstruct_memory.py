"""
Reconstruct Flash contents in order, from SPI CSV readout
"""
# Parse 24 bit address and store it as hex integer in list
def parse_address():
	line = csv_file.readline()				# First address line
	addr1 = line.split(',')[mosi_col]
	line = csv_file.readline()				# Second address line
	addr2 = line.split(',')[mosi_col]		
	addr2 = addr2[2:]						# Gets rid of 0x
	line = csv_file.readline()				# Third address line
	addr3 = line.split(',')[mosi_col]
	addr3 = addr3[2:]						# Gets rid of 0x
	addr = addr1 + addr2 + addr3			# Concatenate string addresses
	hex_int = int(addr, 16)                 # convert hex string to int
	addresses.append(hex_int)				# Add to list of addresses
	return hex_int
	#print(addr)
	
# Parse 32 bit data and store it as hex string
def parse_data(addr,pid):
	dummy = csv_file.readline()				# Skip dummy data
	
	line = csv_file.readline()				# First byte
	data1 = line.split(',')[miso_col]
	data1 = data1[2:4]						# Get rid of 0x and newline
	line = csv_file.readline()				# Second byte
	data2 = line.split(',')[miso_col]
	data2 = data2[2:4]						# Get rid of 0x and newline
	line = csv_file.readline()				# Third byte
	data3 = line.split(',')[miso_col]
	data3 = data3[2:4]						# Get rid of 0x and newline
	line = csv_file.readline()				# Fourth byte
	data4 = line.split(',')[miso_col]
	data4 = data4[2:4]						# Get rid of 0x and newline
	data = data1 + data2 + data3 + data4


		
	"""
	address: 0x1404
	pid:     45
	address: 0x1404
	pid:     6449
	address: 0x1404
	pid:     19663
	"""	
	data_arr.append(data)                   # Add to list of data
	#print(data)
	
# Possible columns we'll read
pid_col = 1
mosi_col = 2
miso_col = 3

# Possible commands to encounter
fast_read = "0x0B"
deepPD = "0xB9"
relPD = "0xAB"

# Store addresses and data from read memory command
addresses=[]
data_arr=[]


# Construct a memory model before updating it
capacity = 2097152
memory = bytearray(capacity) # memory capacity of SPI is 2097152
index=0
# Populate memory with 0xFF
while (index < (capacity-1) ):
	memory[index]=0xff
	index = index + 1

csv_file = open("spi_analyzer_output.csv",'r')

"""
Go through each packet ID, get it's address and data,
and store it in a list. We skip anything that isn't a fast read.
"""
# f000 0a3c should be at 0x1C
line = csv_file.readline() # First line is header information
line = csv_file.readline() # Strip a line from the loop
prev_pid=1 # Don't want this to equal pid at start of loop
pid=0
while (line):  			                # 710,465 is max pid from csv
	pid = line.split(',')[pid_col]		# Get packet id	
	# Don't read command until pid changes
	if (pid != prev_pid):
		command = line.split(',')[mosi_col] # Get command
		#print(command)
		# Check if fast_read command was read
		if (command == fast_read):
			addr = parse_address()		# parse address and add to list
			parse_data(addr,pid)        # parse data and add to list
		#else:
		#	print(command)
		#	print(pid)
		# else ignore other commands until pid changes
	
	prev_pid = pid		
	line = csv_file.readline()

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
