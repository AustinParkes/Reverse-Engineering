import socket
import struct

# Ensure all bytes are sent over socket. Use message length to ensure
def send_cmd(cmd, msg_len):
    totalsent = 0
    while totalsent < msg_len:
        sent = sock.send(cmd[totalsent:])
        if sent == 0:
            raise RuntimeError("socket connection broken")
        totalsent = totalsent + sent
    return totalsent
        
# Need to receive status
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

def speaker_pause():
    cmd = b"SPKR"               # magic header
    cmd += b"xxxx"              # don't care
    cmd += b"\x01\x00\x00\x00"  # msg_len = 0x1 REQUIRED by pause cmd
    cmd += b"\x03"              # Pause cmd
    msg_len = len(cmd)
    print("PAUSE")
    send_cmd(cmd, msg_len)      
    # Receive a reply from the speaker
    reply = receive(13)         # Clear buffer to receive status
    print(reply)              
    speaker_stat()

def speaker_unpause():
    cmd = b"SPKR"               # magic header
    cmd += b"xxxx"              # don't care
    cmd += b"\x01\x00\x00\x00"  # msg_len = 0x1 REQUIRED by unpause cmd
    cmd += b"\x04"              # Add samples command
    msg_len = len(cmd)          
    print("UNPAUSE")
    send_cmd(cmd, msg_len)
    # Receive a reply from the speaker
    reply = receive(13)         # Clear buffer to receive status
    print(reply)
    speaker_stat()

def speaker_stop():
    cmd = b"SPKR"               # magic header
    cmd += b"xxxx"              # don't care
    cmd += b"\x01\x00\x00\x00"  # msg_len = 0x1 REQUIRED by stop cmd
    cmd += b"\x05"              # Add samples command
    msg_len = len(cmd)
    print("STOP")
    send_cmd(cmd, msg_len)
    # Receive a reply from the speaker
    reply = receive(13)
    print(reply)
    speaker_stat()

def speaker_stat():
    cmd = b"SPKR"               # magic header
    cmd += b"xxxx"              # Don't care
    cmd += b"\x01\x00\x00\x00"  # msg_len = 1 REQUIRED by status command
    cmd += b"\x06"              # Status command (the message)
    msg_len = len(cmd)
    send_cmd(cmd, msg_len)
    status = receive(17)
    print("STATUS: %s" % (status))
    return status[16]

def add_samples(audio_data):
    cmd = b"SPKR"               # magic header
    cmd += b"xxxx"              # don't care
    cmd += b"\x01\x80\x00\x00"  # msg_len = 0x8001 REQUIRED by add_samples cmd
    cmd += b"\x02"              # Add samples command
    cmd += audio_data           # audio data
    msg_len = len(cmd)
    print("ADD SAMPLES")
    send_cmd(cmd, msg_len)
    # Receive a reply from the speaker
    reply = receive(13)
    print(reply)

def speaker_init():
    # Speaker Init Command
    cmd = b"SPKR"                # magic header
    cmd += b"xxxx"               # Don't care
    cmd += b"\x05\x00\x00\x00"   # msg_len = 5 REQUIRED by init command
    cmd += b"\x00"               # Init command (part of msg_len)
    cmd += b"\x00\xAC\x00\x00"   # message - sample rate of 44100Hz 
    msg_len = len(cmd)           # (17, 12 for command field, 5 for command + message)
    print("SPEAKER INIT")
    send_cmd(cmd, msg_len)
    # Receive a reply from the speaker
    reply = receive(13)
    print(reply)
    speaker_stat()

def speaker_play():
    cmd = b"SPKR"
    cmd += b"xxxx"
    cmd += b"\x01\x00\x00\x00"
    cmd += b"\x01"
    msg_len = len(cmd)
    print("PLAY")
    send_cmd(cmd, msg_len)
    # Receive a reply from the speaker
    reply = receive(13)
    print(reply)
    speaker_stat()

sock = socket.socket()

# Ethernet TCP socket to communicate with speaker
sock.connect(('192.168.2.10', 4242))


# Audio file to stream
audio_file = open("audio_u16le_44100", "rb")

# Initialize speaker at 44100hz to play sounds
speaker_init()

# Allow speaker samples to be added
speaker_play()

# Read 0x8000 bytes from audio file
audio_data = audio_file.read(0x8000)
stat = 1
count  = 0
while (audio_data):
    
    # Demonstrate pause, unpause, stop
    if (count == 2000): 
        speaker_pause()
    if (count == 3000):
        speaker_unpause()
    if (count == 10000):
        speaker_stop()

    # 1 - Speaker ready for more data, else - not ready
    if (stat == 1): 
        # Add audio samples to the stream
        add_samples(audio_data)
        audio_data = audio_file.read(0x8000)
        
    # Speaker status    
    stat = speaker_stat()
    count = count + 1


audio_file.close()
sock.close()
