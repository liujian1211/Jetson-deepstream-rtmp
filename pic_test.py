# import asyncio
# import socket
# import websockets
# import cv2 as cv
# import threading
# import time

# cap = cv.VideoCapture(0)
# print(cap)
# cap.set(3,480)
# cap.set(4,640)

# def putDate(frame):
#     font = cv.FONT_HERSHEY_SIMPLEX
#     datet = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
#     cv.rectangle(frame, (0,0), (320, 22), (0, 0, 0), -1)
#     frame = cv.putText(frame, datet, (0, 20), cv.FONT_HERSHEY_COMPLEX_SMALL, 1.3,
#                         (255, 255, 255), 2, cv.LINE_4)

#     return frame

# def getJPG():
#     try:
#         ret, frame = cap.read()
#         if not ret:
#             print("Fking无法抓帧")
#         return cv.imencode('.jpg', frame)[1].tobytes()  # cv2转Bytes
#     except Exception as e:
#         print(e.args)

# def MAIN():
#     try:
#         client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#         host = "10.0.1.34"  #改成服务器公网IP
#         port = 8888         #服务器开放的对应的端口
#         print('正在连接。。。')
#         client.connect((host, port))
#         print("连接成功,WORKING~")
#         while True:
#             time.sleep(0.1)
#             # img = getJPG()
#             # _, img = cap.read()  #实时流

#             img = cv.imread('/home/nano/liujian/test_websocket/yolov3.jpg')

#             # img = cv.flip(img, 1)
#             img = putDate(img)
#             cv.imshow('JNano', img)
#             if cv.waitKey(1) & 0xFF == ord('q'):
#                 break
#             # 压缩图片
#             _, send_data = cv.imencode('.jpg', img, [cv.IMWRITE_JPEG_QUALITY, 50]) # jpg编码压缩
#             client.sendto(send_data, (host, port))
#     except Exception as e:
#         print("寄掉了")
#         print(e.args)
#         print("即将重试")
#         time.sleep(3)
#         threading.Thread(target=MAIN).start()

#     client.close()


# threading.Thread(target=MAIN).start()

import serial
import pynmea2
import time

ser = serial.Serial("/dev//ttyUSB0",9600)
while True:
    line = ser.readline()
    if line.startswith('$GNRMC'):
        rmc = pynmea2.parse(line)
        print ("Latitude:  ", float(rmc.lat)/100)
        print ("Longitude: ", float(rmc.lon)/100)
        break 


# if __name__ == '__main__':
#     MAIN()