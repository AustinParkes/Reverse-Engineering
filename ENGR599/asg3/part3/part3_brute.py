import serial
import time
from itertools import product
ser = serial.Serial("/dev/ttyACM0", 38400, timeout=0.1)

# Strings we expect to receive from analyzing the binary
prompt = b"Please enter the password:\r\n"
incorrect = b"Incorrect password!\r\n"
success = b"SUCCESS!\r\n"

passwords = []                  # Store succesful passwords
attempt=1                       # Keep track of the # of password attempts
hits = []                       # Keep track of the attempts it worked on
timekeeping = []                # keep track of the times it occurred at

elements=1                      # num of characters to fuzz at a time

# Roughly calculate the run-time in seconds 
start = time.time()

# Characters to fuzz with
pw_data="1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" 

# Try 240,000 passwords. About the amount of time expected
# to for sure get a hash hit
while (attempt <= 240001):
   # Iterate through ALL possible product of the data
   if (elements<=len(pw_data)):
      # Would do this differently another time, since this creates exponentially massive lists and is memory exhaustive
      products = list(product(pw_data, repeat=elements)) # generate list of all products 
      elements=elements+1                                  
      # Split up the tuples in the list generated
      for tup in products:
         fuzz = ''.join(tup)         # Join the strings in the tuples
         fuzz = fuzz + '\n'          # Needs a terminator!
         fuzz = fuzz.encode('utf-8') # Encode string to byte string
         ser.write(fuzz)  
         print(fuzz)
         receive = ser.readline()    # Save terminal reply
         print(receive)                
         # Check the UART response
         if (receive == incorrect):
            receive = ser.readline()    # empty buffer containing the prompt before continuing to fuzz
            print(attempt,": incorrect")
            attempt=attempt+1           # add to attempt counter
         elif (receive == success):
            receive = ser.readline()    # empty buffer containing the prompt before continuing to fuzz
            print(attempt,": success")
            print(fuzz)                 # prints the password
            curr_time = (time.time() - start)
            timekeeping.append(curr_time)
            passwords.append(fuzz)      # Save successful passwords
            hits.append(attempt)        # Save succesful attempts
            attempt=attempt+1           # add to attempt counter
         else:
            print("Failure to receive")
            print(receive)


# Diplay all of the successful passwords and attempts
print(passwords)
print(hits)
print(timekeeping)

