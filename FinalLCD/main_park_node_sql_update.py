import time
import serial
from sql_conn_park_node import sql_class

arduino = serial.Serial(port="/dev/ttyUSB0", baudrate=4800)
time.sleep(3)

def Decode(A):
    A = A.decode()
    A = str(A)

    if A[0]=='P':                       #첫문자 검사   A = P_OFF_a0b0  or  P_ON!_a0b0
        Ard1=str(A[:5])             # Ard1 = P_OFF or P_ON! -- 조도센서로 차량의 유무 파악!!
        result = Ard1

        sql = sql_class()

        if result[3]=='F':  #차량이 없는 상태로 바뀌었다면 (차량이 있는 상태에서 없는 상태로 변경)  조도센서 On -> OFF

            Ard2=str(A[6:8])  # ex) a1
            Ard3=str(A[8:10])   # ex) b1

            parking_space = Ard2    # ex) a1
            level = Ard3    # ex) b1
            id_loc = str(parking_space + level)    # ex) a1b1
            availability = 1   # 비어있음, 이용 가능
            status = "EMPTY"

            # print("result :", Ard1, "parking_space : ", Ard2, "level : ", Ard3)

            sql.carout_update_SQL(id_loc, availability, status)  # On -> OFF로 변화!!
            
            return result
            
            
        else:
            Ard2=str(A[6:8])
            Ard3=str(A[8:10])

            parking_space = Ard2
            level = Ard3
            id_loc = str(parking_space + level)

            status = "FILL"

            #print("result :", Ard1, "id_loc :", id_loc)

            sql.carin_update_SQL(id_loc, status)  # OFF -> ON로 변화!!
            
            return result

    else :
        print ("Error_Wrong Signal")
        return False
    
def Ardread(): # return list [Ard1,Ard2]
        if arduino.readable():
            LINE = arduino.readline()
            code=Decode(LINE) 
            print(code)
            return code
        else : 
            print("읽기 실패 from _arduinoread_")


while (True):
    Ardread()
    
    # parking = ['P_ON!']
    # if '!' in result:
    #     print(sql_class.select_all)