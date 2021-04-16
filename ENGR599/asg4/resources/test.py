def func1():
	line = csv_file.readline()
	print(line)


def func2():
	line = csv_file.readline()
	print(line)

csv_file = open("spi_analyzer_output.csv",'r')

line = csv_file.readline()
print(line)

func1()
func2()
