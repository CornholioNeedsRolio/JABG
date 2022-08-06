#!/usr/bin/python

import os
import pickle
import sys
from threading import Thread

#-ggdb -g3 
buildDir = 'build'
normieMode = False
compiler = "g++ "
extension = ".out"
link_libs = '-lSDL2 -lGL -lGLU -ldl -lfreetype'
paramaters = '-ggdb -g3 -O3 -Wall -Wno-reorder -Wno-sign-compare -Wno-unused-command-line-argument -march=native -pthread -std=c++17 -I./include -I/usr/include/freetype2/'
if("--normie" in sys.argv or "-n" in sys.argv):
    normieMode = True
    buildDir = 'winbuild'
    compiler = "/opt/msvc/bin/x64/cl "
    extension = ".exe"
    #paramaters = '-g -Wall -Wno-reorder -Wno-sign-compare -O3 -Wno-unused-command-line-argument -pthread -std=c++17 -I./include -I/usr/include/freetype2 -L/usr/x86_64-w64-mingw32/lib -os=windows'
    #link_libs = '-mwindows -lstdc++ -lopengl32 -lglu32 -lSDL2 libfreetype.a'
outputFile = 'jabg'+extension


def occurrences(file, word):
    total = 0
    with open(file) as file:
        for line in file.readlines():
            if '.h' in line:
                continue
            found = line.find(word)
            if found != -1 and found != 0:
                total += 1
    return total

def getClassesAndStructs(file):
    classes = []
    with open(file) as file:
        for line in file.readlines():
            list_of_keywords = line.split()
            pos = 0
            found = False
            for keyword in list_of_keywords:
                pos = pos + 1
                if keyword == 'class' or keyword == 'struct':
                    found = True
                    break
            if(found):
                classes.append(list_of_keywords[pos])

    return classes 
                

def getLastEdited(file):
    time = os.path.getmtime(file)
    with open(file) as cpp_file:
        for line in cpp_file.readlines():
            if line.startswith("#include"):
                line.rstrip()
                quotes = 2
                header_name = ""
                for letter in line:
                    if(letter == '<'):
                        break
                    if(letter == '\"'):
                        quotes = quotes - 1
                    if(quotes == 0):
                        break
                    if(quotes == 1 and letter != '\"'):
                            header_name = header_name + letter
                if(header_name.endswith(".h")):
                    os_path = os.path.dirname(file)
                    if(os_path == ''):
                        os_path = os.path.dirname(__file__)
                    header_name = os.path.realpath(os_path + "/" + header_name)

                    classes = getClassesAndStructs(header_name)
                    hasOccurences = False
                    for keyword in classes:
                        occur = occurrences(file, keyword)
                        if occur > 0:
                            hasOccurences = hasOccurences or True
                    if hasOccurences:
                        header_time = getLastEdited(header_name)
                        time = max(time, header_time)

    return time

f = []
for path, subdirs, files in os.walk("./src/"):
    for name in files:
        if name.endswith('.cpp') or name.endswith('.c'):
            f.append(os.path.join(path, name).replace('./', ''))
f.append("./libs/glad/glad.c")
try:
    os.mkdir(buildDir)
    print('Directory', buildDir, 'has beed created')
except FileExistsError:
    print('Directory', buildDir, 'already exists')


print('Building object files')
objects = ""
shell_command = ""
obj_builded = 0
obj_total = 0


workingDirWindows = os.getcwd().replace('/', '\\\\')
def compObj(file):
    global objects
    global obj_total
    global obj_builded

    obj_total = obj_total + 1
    obj = buildDir + '/' + os.path.basename(file).replace('.cpp', '').replace('.c', '')
    if(normieMode):
        obj += '.obj'
    else:
        obj += '.o'
    objects += obj + ' '
    #if file in lastEdited:
    #    if lastEdited[file] == time and ("-b" not in sys.argv):
    #        continue
    try:
        time = getLastEdited(file)
        if time < os.path.getmtime("./"+obj) and ("-b" not in sys.argv):
            return
    except:
        pass

    obj_builded = obj_builded + 1
    output = ""
    if(normieMode):
        output = os.popen(compiler+" /O2x /EHsc /experimental:module /std:c++latest /I "+workingDirWindows +"\\\\libraries\\\\include" + " /I "+workingDirWindows + "\\\\src "+ "/Fo /c " + workingDirWindows +"\\\\{}".format(file.replace('/', '\\\\'))).read()
        try:
            os.replace(os.getcwd()+'/'+os.path.basename(obj), obj)
        except:
            pass
    else:
        output = os.popen(compiler + '-c -o ' + obj + ' ' + file + ' ' + paramaters).read()
    print(file + ':' + output)

threads = []
for file in f:
    process = Thread(target=compObj, args=[file])
    process.start()
    threads.append(process)

for process in threads:
    process.join()

print('Finished building object files, building executable now, builded', obj_builded, '/', obj_total)
paramaters = paramaters + ' ' + link_libs 
if(normieMode):
    os.system(compiler+" /Otxdiy /std:c++latest {} /o{} /link /LIBPATH:{}\\\\libraries\\\\lib\\\\ /SUBSYSTEM:CONSOLE /DEFAULTLIB:SDL2.lib /DEFAULTLIB:SDL2main.lib /DEFAULTLIB:opengl32.lib /DEFAULTLIB:freetype.lib".format(objects, outputFile, workingDirWindows))
else:
    os.system(compiler + '-o ' + outputFile + ' ' + paramaters + ' ' + objects)

if(("-r" in sys.argv)):
    print('Running program')
    if(normieMode):
        os.system("wine ./" + outputFile)
    else:
        if("-d" in sys.argv):
            os.system('gdb '+outputFile)
        else:
            os.system('./'+outputFile)

os.system('echo ' + paramaters)