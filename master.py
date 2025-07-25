import os
import sys

if len(sys.argv) != 2:
    print(
        '''Usage :
        python master.py cmake 
        python master.py make
        python master.py build 
        python master.py run
        python master.py all 
        python master.py binlist'''
    )
    exit(1)

def cmake():
    return os.system('cmake -S . -B build -G "Unix Makefiles"')

def make():
    return os.system('cd build && make')

def copy_source():
    if sys.platform == 'linux':
        return os.system('cp main.dim build/bin')
    return os.system('copy main.dim build\\bin\\main.dim')

def build():
    err = cmake()
    if err != 0:
        return err

    err = make()
    if err != 0:
        return err

    return copy_source()

def run():
    if sys.platform == 'linux':
        return os.system('cd build/bin && ./dim-lang')
    return os.system('cd build/bin && .\\dim-lang')

def all():
    err = build()
    if err != 0:
        return err
    return run()

def binlist():
    print('\n'.join([f'- {item}' for item in os.listdir('build/bin')])) 

args = {
    'cmake': cmake,
    'make': make,
    'build': build,
    'run': run,
    'all': all,
    'binlist': binlist
}

action = sys.argv[1]

args[action]()

