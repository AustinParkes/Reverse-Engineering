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
def parse_data(b,pid):
	dummy = csv_file.readline()				# Skip dummy data
	line = csv_file.readline()				# First data line
	data1 = line.split(',')[miso_col]
	data1 = data1[2:4]						# Get rid of 0x and newline
	line = csv_file.readline()				# Second data line
	data2 = line.split(',')[miso_col]
	data2 = data2[2:4]						# Get rid of 0x and newline
	line = csv_file.readline()				# Third data line
	data3 = line.split(',')[miso_col]
	data3 = data3[2:4]						# Get rid of 0x and newline
	line = csv_file.readline()				# Fourth data line
	data4 = line.split(',')[miso_col]
	data4 = data4[2:4]						# Get rid of 0x and newline
	data = data1 + data2 + data3 + data4
	if (data == "F0000A3C"):
		print(data)
		print(b)
		print(pid)
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


csv_file = open("spi_analyzer_output.csv",'r')


line = csv_file.readline() # First line is header information
line = csv_file.readline() # Strip a line from the loop

prev_pid=1 # Don't want this to equal pid at start of loop
pid=0
while (line):
	pid = line.split(',')[pid_col]		# Get packet id	
	# Don't read command until pid changes
	if (pid != prev_pid):
		command = line.split(',')[mosi_col] # Get command
		#print(command)
		# Check if fast_read command was read
		if (command == fast_read):
			b = parse_address()		# parse address and add to list
			parse_data(b,pid)        # parse data and add to list
		# else ignore other commands until pid changes
	
	prev_pid = pid		
	line = csv_file.readline()

#print(addresses[0:20])
#print((data_arr[0:20]))	

# Sort data in order, while filling missing addresses with 0xFF
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
		print(temp_index)
		while (temp_index < len(addresses)):
			if (addresses[temp_index] == 5124 and expected_address==28):
				print(data_arr[temp_index])	
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
	
