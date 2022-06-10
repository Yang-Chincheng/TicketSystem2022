import os
import json
import subprocess
from filecmp import cmp as fcmp
import time

data_dir = 'data/normal'
compile_script = 'build.sh'
run_script = 'run.sh'
clean_script = 'clean.sh'

def printRet(detail=True):
    def decorator(func):
        def wrapper(*args, **kw):
            if detail:
                print("\033[36m{}\033[0m".format('call: {}'.format(func.__name__)))
            ret,val=func(*args, **kw)
            if val!='':
                if ret==0:
                    print("\033[37m{}\033[0m".format(val))
                else:
                    print("\033[31m{}\033[0m".format('ret code: '+str(ret)))
                    print("\033[37m{}\033[0m".format(val))
                    quit()
        return wrapper
    return decorator

@printRet(True)
def compile_project():
    return subprocess.getstatusoutput('sh {}'.format(compile_script))

@printRet(False)
def run(input_path):
    return subprocess.getstatusoutput('sh {} <{} >tmp.out'.format(run_script,input_path))

@printRet(False)
def clean():
    # subprocess.getstatusoutput('rm tmp.out')
    return subprocess.getstatusoutput('sh {}'.format(clean_script))

def main():
    if not os.path.exists(data_dir):
        print("\033[31m{}\033[0m".format('data set not find!'))
        exit(127)
    with open('{}/config.json'.format(data_dir),'r')as f:
        testInfo=json.load(f)

    compile_project()

    test_point = {
        'basic_1',
        'basic_2', 
        'basic_3', 
        'basic_4', 
        'basic_5', 
        'basic_6',
        # 'basic_extra',
        # 'pressure_1_easy', 
        # 'pressure_1_hard', 
        # 'pressure_2_easy',
        # 'pressure_2_hard',
        # 'pressure_3_easy',
        # 'pressure_3_hard',
    }

    errorCase=[]
    for (name, test) in testInfo.items():
        if name not in test_point: continue
        fail_point=None
        print('test for {}'.format(name))
        begintime = time.time()
        for point in test:
            file_path='{}/{}/{}'.format(data_dir,name,point)
            if os.path.exists(file_path+'.in') and os.path.exists(file_path+'.out'):
                starttime = time.time()
                run(file_path+'.in')
                curtime = time.time()
                print(f"point {point} takes {round(curtime - starttime, 2)} secs")
                try:
                    result=fcmp(file_path+'.out','tmp.out')
                except:
                    result=False
                if not result:
                    errorCase.append(name)
                    fail_point=point
                    break
            else:
                print("\033[31m{}\033[0m".format('data set is broken at {}!'.format(file_path)))
                exit(127)
        endtime = time.time()
        print(f"{name} total time {round(endtime - begintime, 2)}")
        clean()
        if fail_point==None:
            print("\033[36m{}\033[0m".format('success in {}'.format(name)))
        else:
            print("\033[31m{}\033[0m".format('case {} fail at {}'.format(name,fail_point)))
            break
    print("\033[36m{}\033[0m".format('pass case {}/{}'.format(len(errorCase),len(testInfo))))

if __name__=='__main__':
    main()

