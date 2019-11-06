import subprocess

myCmd1 = 'export ROS_MASTER_URI=http://192.168.0.123:11311'
myCmd2 = 'export ROS_HOSTNAME=192.168.0.186' 
myCmd3 = 'export ROS_IP=192.168.0.186'
myCmd4 = 'rosrun rospy_tutorials talker.py'

processes = []

p1 = subprocess.Popen(myCmd1, stdout=subprocess.PIPE, shell=True)
p2 = subprocess.Popen(myCmd2, stdout=subprocess.PIPE, shell=True)
p3 = subprocess.Popen(myCmd3, stdout=subprocess.PIPE, shell=True)
p4 = subprocess.Popen(myCmd4, stdout=subprocess.PIPE, shell=True)

processes.append(p1)
processes.append(p2)
processes.append(p3)
processes.append(p4)

for p in processes:
    if p.wait() != 0:
        print("There was an error")

print("all processed finished")
