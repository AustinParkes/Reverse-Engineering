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
	hex_addr = int(addr, 16)                # convert hex string to int
	return hex_addr
	
# Parse data bytes and store in memory
def parse_data(addr,start_pid):
	dummy = csv_file.readline()				# Skip dummy data
	line = csv_file.readline()	
	# Get bytes data from an address
	while(1):    
	                   
		# Cycle through the bytes until pid changes            		
		data = line.split(',')[miso_col]         # Get data as a hex string
		data = data[2:4]						 # Get rid of 0x and newline
		memory[addr] = int(data,16)              # Store byte into memory model
		
		line = csv_file.readline()
		
		# Leave if nothing more to read
		if (not(line)):
			exit=-1
			return exit, line	
			
		# Check if pid has changed
		pid = line.split(',')[pid_col]	
		
		
    	# Keep storing data until pid changes
		if (pid == start_pid):    
			addr = addr + 1
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

# Strip some variables from the first iteration
line = csv_file.readline() 			# header information
line = csv_file.readline() 			# Strip a line from the loop
pid = line.split(',')[pid_col]		# packet id	

prev_pid=1 	 # Value 	
pid=str(0)   # pid is string thru program    
while (pid != -1):  			            	# -1 is exit condition
	if (pid != prev_pid):						# Don't read a command until pid changes	
		command = line.split(',')[mosi_col] 	# Get command
		if (command == fast_read):
			addr = parse_address(command)		# parse address from 'read' command
			prev_pid = pid
			pid, line = parse_data(addr, pid)   # parse data from address(es) and store in memory model
		else:                              		# Read lines until pid changes
			# OTHER COMMANDS IDENTIFIED HERE with 'print(command)'	
			line = csv_file.readline()
			pid = line.split(',')[pid_col]		# Get packet id	


f = open("opdump.bin", 'wb')
f.write(memory)
f.close

