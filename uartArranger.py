import Adafruit_BBIO.UART as UART
import serial
import time
UART.setup("UART1")
serial.PARITY_SPACE

from collections import OrderedDict

ser = serial.Serial(port= "/dev/ttyO1", baudrate=9600)
ser.close()
ser.open()
#When the serial is opened, get the data from the terminal
if ser.isOpen():
   print "Serial is open"

   data=ser.read()
   time.sleep(1)
   data +=ser.read(ser.inWaiting())
   uartList=list(OrderedDict.fromkeys(data))
    
   uartList1=list(data)
   
   print len(uartList)
   
   
def uart_discri(list1):
 '''This function is to test the gotten data and discriminate the unwanted data.
   If the number of element of the data is less than 3, the data is not reliable.'''
 #The if statements that makes sure it is less than 3 element . 
  if len(list1)<=3:
     list2=list(list1[0:2])
     list3=list(list1[3:5])

     if(list2[0]==list3[0] and list2[1]==list3[1]):
        return list2

     list4=list(list1[0:3])
     list5=list(list1[4:7])
     if(list4[0]==list5[0] and list4[1]==list5[1] and list4[2]==list5[2]):
        return list4
	
     list6=list(list1[0:4])
     list7=list(list1[5:9])
     if (list6[0]==list7[0] and list6[1]==list7[1] and list6[2]==list7[2] and list6[3]==list7[3]):
		return list6
		
     list8=list(list1[0:5])
     list9=list(list1[6:11])
     if (list8[0]==list9[0] and list8[1]==list9[1] and list8[2]==list9[2] and list8[3]==list9[3] and list8[4]==list9[4]):
		return list8		
   else:
       print("Data not correct\n")	 

.
	   
def uart_zee(unarrangedList):
   '''This function is to arrange the data as it is suppose to be from the other micro-controller. 
The ring buffer sends signals in a FIFO format "Z" is assigned as the start of the data'''
   if( "Z" in unarrangedList):
       print ("Yes")
       d = unarrangedList.index("Z")
       print d

       if (d ==0):
            del unarrangedList[0]
			return unarrangedList
            #for i in unarrangedList:
               #print i

       elif (d==1):

          unarrangedList.append(unarrangedList[0])
          del unarrangedList[0]
          del unarrangedList[0]
          return unarrangedList
          #for i in unarrangedList:
           #print i
       elif (d==2):
           unarrangedList.append(unarrangedList[0])
           unarrangedList.append(unarrangedList[1])
           del unarrangedList[0]
           del unarrangedList[0]
           del unarrangedList[0]
		   return unarrangedList
           #for i in unarrangedList:
            # print i
       elif (d==3):
          unarrangedList.insert(0,unarrangedList[4])
          unarrangedList.insert(0, unarrangedList[4])

          del unarrangedList[6]
          del unarrangedList[5]
          del unarrangedList[0]
		  return unarrangedList
          #for i in unarrangedList:
              # print i
       elif (d==4):
	   del unarrangedList[4]
       return unarrangedList
          #for i in unarrangedList:
            # print i
   else:
     print ("Incorrect data has been sent across i.e Z is missing\n")
 
	
	
def uart_compare(Listing):
       '''This function is to convert from string to integers.
       It works for four digits numbers.
       It compares the index of the array to the alphabet generated in list alphabet.'''	
       alphabet=["A","B","C","D","E","F","G","H","I","J"]
       
       answerLength =len(Listing)

       if(answerLength==4):
	      aaaa = alphabet.index(Listing[0])
          bbbb = alphabet.index(Listing[1])
          cccc = alphabet.index(Listing[2])
          dddd = alphabet.index(Listing[3])

          result1 =aaaa*1000 + bbbb*100 + cccc*10 + dddd*1
          print result1
       if(answerLength==3):
	      aaaa = alphabet.index(Listing[0])
          bbbb = alphabet.index(Listing[1])
          cccc = alphabet.index(Listing[2])    
          result=aaaa*100 + bbbb*10 + cccc*1
          print result
       if(answerLength==2):
	      aaaa = alphabet.index(Listing[0])
          bbbb = alphabet.index(Listing[1])  
          result =aaaa*10 + bbbb
          print result
       if(answerLength==1):
	      aaaa = alphabet.index(Listing[0])    
          result = aaaa
          print result
	   
	   
if __name__ == "__main__":		

answer1 =uart_discri(uartlist1)
answer2 = uartList

if( answer1==answer2):
    print("Perfect job\n")
else:
    print("Something is wrong somewhere\n")

uart_compare(answer2)	
