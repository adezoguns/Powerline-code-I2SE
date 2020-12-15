import Adafruit_BBIO.UART as UART
import serial
import time
UART.setup("UART1")

i=0
m=0
crcPoly=0x91
crc=0
n=[]
relData=[]
listChar=[]
hexaChar=[]
indata=[]
message=[]


hexaList=""

charaString=""
myData=""
#input_char

ser = serial.Serial(port= "/dev/ttyO1", baudrate=9600, timeout=0)
ser.close()
ser.open()

if ser.isOpen():

  print "Serial is open"

def norm_char():
     
     '''It sends normal characters typed into the terminal'''

     global input_char
     input_char=raw_input("Enter data here: ")
     return input_char
  

def uharty():
   
     '''It takes characters typed into the terminal and convert it to hexadecimal string'''

     i=0
     listChar=[]
     listHexa=[]
     charHexa=""
  
     writeChar=raw_input("Enter data here: ")
     listChar=list(writeChar)
     charLen =len(listChar)
      
     while i<charLen:
          listHexa.append(hex(ord(listChar[i])))
          i=i+1
     charHexa=' '.join(hexaChar)
     return charHexa      


def mod_data():

     '''It takes characters typed into the terminal and convert it to modbus format string'''

     i=0
     listChar=[]
     hexaList=['0x3a']
     hexaChar=" "
     listAns=[]
     CR=13
     LF=10
     modHex=[]

     writeChar=raw_input("Enter data here: ")
     listChar=list(writeChar)
     charLen =len(listChar)

     while i<charLen:
          hexaList.append(hex(ord(listChar[i])))
          i=i+1
          #print hexaList

     for i in range (0, len(hexaList)):
             listAns.append((int(hexaList[i],16)))

     #Removing the start symbol(:,'0x3a','58') before performing CRC
      
     time.sleep(0.2)
     listAns.remove(58)
     
     #Perform CRC
     
     time.sleep(0.2)
     q=crcCheck(listAns)
     
     #Insert the start symbol into the array(:,'0x3a','58')
     
     time.sleep(0.2)
     listAns.insert(0,58)
   
     '''Adding crc to the message to be sent through modbus'''
     
     time.sleep(0.2)
     listAns.append(q)

     '''Ending the modbus transmission(CRLF)'''

     time.sleep(0.2)
     listAns.append(CR)
     listAns.append(LF)     

     #print listAns
     fb= ' '.join([hex(i) for i in listAns]) 
     return fb  
 
def hex_to_dec(relData):
   
     '''It takes hexadecimal and converts it to a decimal string'''
   
     charaString=(''.join(chr(i) for i in relData))
     return charaString

def to_array():

    '''It convert data into Array Hexadecimal'''

    b=3
    n2=""
    n3=[]
    n4=[]
    n5=[]
    n6=[]    
    n7=[]
    counter=0

    n=list(uharty())
    n2=''.join(n)
    n2=n2.replace(',',' ')
    n3=list(n2)
    si= iter(n3)
    n4= map(str.__add__,si,si)
    si2=iter(n4)
    n5=map(str.__add__,si2,si2)
    for i in range (0, len(n5)):
        n6.append(int(n5[i],16))
    #('[{}]'.format(', '.join(hex(ord(n6)) for x in len(n6))))
    print n6
  
def crc_check( indata):

   '''It performs Cyclic Redundancy Check (CRC) on a given data list'''
   
   crc=0
   i=0
   j=0
   
   for i in range (0, len(indata)):
         crc^= indata[i]
         for j in range(0,8):
            
 	    if(crc & 0x01)==0x01:   
         	      crc ^= crcPoly
	    crc >>=1           
   return crc

def crc_plus_message(message):

  '''It adds the CRC to the data to be sent they are both in hexadecimal'''
  
  hexMess=[] 
  p=crcCheck(message)
  message.append(p)
  for i in range (0, len(message)):
      hexMess.append(hex(message[i]))
  #print hexMess
  return hexMess

def messenger():

    message1= normChar()
    message2= str(crcCheck(toArray()))
    print (message1 + message2)

    
def read_data():

     '''It reads data from the UART'''

     #ser.flushOutput()
     #ser.flushInput()
     readMyData=ser.readline()
     print readMyData

def write_data(myData):

       '''It writes array data to the UART'''
       
       #ser.flushOutput()
       #ser.flushInput()
       myStr=""
       myStr=''.join(myData)
       ser.write(myStr)
       time.sleep(1)

def write_data_str(StrData):
    '''It writes string data to the UART'''
    ser.write(StrData)
    time.sleep(1) 

##########################################################################################
##########################################################################################
#if __name__=="__main__":

while 1:
    #to_array()
    #hexToDec([0x45, 0x7e, 0x3])
    #write_data(crc_plus_message(to_array()))
    #crc_plus_message([0x83,0x01])
    #crc_check(to_array())
    write_data_str(modData())
    #uharty()
    #messenger()
    read_data()
    #mod_data()
    #mod_array()
