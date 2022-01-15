#!/usr/bin/python

import os
import pickle
import sys

buildDir = 'build'
normieMode = False
compiler = "g++ "
extension = ".out"
link_libs = '-lSDL2 -lGL -lGLU -ldl -lfreetype'
paramaters = '-Wall -Wno-reorder -Wno-sign-compare -O3 -Wno-unused-command-line-argument -march=native -pthread -std=c++17 -I./include -I/usr/include/freetype2/ '
to_executable = ""
if("--normie" in sys.argv or "-n" in sys.argv):
    normieMode = True
    compiler = "x86_64-w64-mingw32-g++ "
    extension = ".exe"
    link_libs = '-lmingw32 -lSDL2 -lfreetype -mwindows -lstdc++ -lopengl32 -lglu32'
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
for path, subdirs, files in os.walk("./"):
    for name in files:
        if ('.cpp' in name) or ('.c' in name):
            f.append(os.path.join(path, name).replace('./', ''))

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
for file in f:
    obj_total = obj_total + 1
    obj = buildDir + '/' + os.path.basename(file).replace('.cpp', '').replace('.c', '') + '.o'
    objects += obj + ' '
    #if file in lastEdited:
    #    if lastEdited[file] == time and ("-b" not in sys.argv):
    #        continue
    try:
        time = getLastEdited(file)
        if time < os.path.getmtime("./"+obj) and ("-b" not in sys.argv):
            continue
    except:
        pass

    obj_builded = obj_builded + 1
    print(file + ':')
    os.system(compiler + '-c -o ' + obj + ' ' + file + ' ' + paramaters)

print('Finished building object files, building executable now, builded', obj_builded, '/', obj_total)
paramaters = paramaters + ' ' + link_libs 
os.system(compiler + '-o ' + outputFile + ' ' + paramaters + ' ' + objects)

if(("-r" in sys.argv)):
    print('Running program')
    if(normieMode):
        os.system('wine '+outputFile)
    else:
        if("-d" in sys.argv):
            os.system('gdb '+outputFile)
        else:
            os.system('./'+outputFile)

os.system('echo ' + paramaters)