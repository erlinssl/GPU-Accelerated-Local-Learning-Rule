import datetime
import os
import subprocess

branches = ["main", "compute", "threads-v1"]

file = open("./stats/" + datetime.datetime.now().isoformat(), "a")
os.chdir("build")

os.system("cmake ..")

os.system("make")

command = "./filter_finder 1.0 0.5 10 4 1000 5 0.1".split()
for i in branches:
    os.system("git checkout " + i)
    file.write("\n" + i + "," + subprocess.run(command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE).stderr.decode("utf-8"))
