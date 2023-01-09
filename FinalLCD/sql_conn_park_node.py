# import rospy
import mysql.connector
import datetime
import time
import serial

class sql_class:
    def __init__(self):
        super(sql_class, self).__init__()
        print("here to connect SQL")
        # connect database
        self.remote = mysql.connector.connect(
            host = "",  // AWS rds end point
            port = 3306,   // port number
            user = "",     //User Name
            password = "",   //User Password
            database = ""   / Database Name
        )
        self.cursor = self.remote.cursor(buffered=True)
        # sql query
        self.query_head = """UPDATE iot_project SET """ 
        self.query_tail = """ WHERE ID_LOC = """


    def carout_update_SQL(self, id_loc, availability, status):
        print("here to update SQL")

        sql = self.query_head + "START_TIME = NULL" + ", CAR_NUM = NULL" + \
            ", AVAILABILTY = " + str(availability) + ", STATUS = '" + \
            status + "'" + self.query_tail + "'" + id_loc + "';"
        print(sql)
        self.cursor.execute(sql)
        self.check_Query(id_loc)
        self.remote.commit()

    def carin_update_SQL(self, id_loc, status):
        print("here to update SQL")
        # id_loc = parking_space + level

        sql = self.query_head + "STATUS = '" + status + "'" + self.query_tail + "'" + id_loc + "'"  ";"
        print(sql)

        self.cursor.execute(sql)
        self.check_Query(id_loc)
        self.remote.commit()
    
    def update_SQL(self, level, parking_space, start_time, car_num, availability, status):
        print("here to update SQL")
        start_time = str(datetime.datetime.fromtimestamp(start_time.to_sec()))[:19]
        # level --> b1
        # parking_space --> a1
        # start_time: DATETIME
        # car_num: VARCHAR(45)
        # availability: TINYINT --> 0: not available || 1: available
        # status: VARCHAR(45)  --> EMPTY || MOVING || FILL
        sql = self.query_head + "START_TIME = '" + start_time + "', CAR_NUM = '" + \
            car_num + "', AVAILABILTY = " + str(availability) + ", STATUS = '" + \
            status + "'" + self.query_tail + "'" + parking_space + "';"
        print(sql)
        self.cursor.execute(sql)
        self.check_Query(parking_space)
        self.remote.commit()
    
    def check_Query(self, id_loc):
        sql = """SELECT * from iot_project where ID_LOC = '""" + id_loc + "';"
        self.cursor.execute(sql)
        result = self.cursor.fetchall()
        for result_iterator in result:
            print(result_iterator)

    
    def select_all(self):
        sql = """SELECT * from iot_project"""
        self.cursor.execute(sql)
        self.result = self.cursor.fetchall()
        for result_iterator in self.result:
            print(result_iterator)

    def disconnect_SQL(self):
        print("here to disconnect SQL")
        # disconnect database
        self.remote.close()

# if __name__ == "__main__":
#     rospy.init_node('sql_connect', anonymous=False)
#     sql = sql_class()



        
