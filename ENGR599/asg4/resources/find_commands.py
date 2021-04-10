"""
Find list of commands in the csv file
NOTE: Script doesn't really work as intended, but it found all the commands so stopping here!!
"""
# Possible columns we'll read
pid_col = 1
comm_col = 2
data_col = 3


csv_file = open("spi_analyzer_output.csv",'r')

# First line is column information
line = csv_file.readline()
line = csv_file.readline() # strip a line from the loop

prev_pid=0
pid=0
list_index=0
while (line):
	
	pid = line.split(',')[pid_col]
	
	# Command if new pid
	if (prev_pid != pid):
		command = line.split(',')[comm_col]
		if (command != command_list[list_index]):
			command_list.append(command)
			list_index = list_index + 1
				
		prev_pid = pid
	line = csv_file.readline()   # file line to read columns from


print(command_list)	
print("Done")
	
