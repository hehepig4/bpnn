import os
#change to readonly
import time
test_dir='OS/week17/data'
def readFileToString(file_name):
    filehandle=open(file_name,"r")
    str=filehandle.read().strip(' ')
    str=str.strip('\n')
    str=str.strip('\r')
    str=str.strip('\t')
    filehandle.close()
    return str

def getFulldir(dir):
    dirs=os.getcwd()
    return os.path.join(dirs,dir)

def walkDir(dir_name):
    
    dir_list=os.listdir(dir_name)
    return_dict={}
    for id in dir_list:
        if id not in score_dict.keys():
            continue
        full_path=getFulldir(dir_name+'/'+id)
        files=[]
        for root,dir,filesp in os.walk(full_path):
            for file in filesp:
                fpath=os.path.join(root,file)
                timestr=time.localtime(os.path.getmtime(fpath))
                timestr=time.strftime('%Y-%m-%d %H:%M:%S',timestr)
                files.append([fpath,timestr])
    return_dict.update({id:files}) 
    return return_dict

def isSame(file_dir1,file_dir2):
    return readFileToString(file_dir1)==readFileToString(file_dir2)

constant_dict={}
score_dict={}
stufile_dict={}
testid_scores={1:60,2:30}


def ifAfterDate(date1,date2='2022-05-26 23:59:59'):
    d1=time.strptime(date1,'%Y-%m-%d %H:%M:%S')
    d2=time.strptime(date2,'%Y-%m-%d %H:%M:%S')
    return time.mktime(d1)>time.mktime(d2)

def initScore(start_stu,end_stu):
    score_dict.update({str(id):0 for id in range(start_stu,end_stu+1)})
    constant_dict.update({str(id):1 for id in range(start_stu,end_stu+1)})
    
def checkSameWithFileName(files_name_list):
    for file_name in files_name_list:
        compare_list=[]
        for stu in stufile_dict.keys():
            exist_flag=False
            for file in stufile_dict[stu]:
                if file[0][-len(file_name):]==file_name:
                    exist_flag=True
                    compare_list.append([stu,file[0],file[1]])
                    break
            if not exist_flag:
                constant_dict[stu]*=0
        for stu,_,cname_t in compare_list:
            if ifAfterDate(cname_t):
                constant_dict[stu]*=0.9
        
        for (stu1,file_name1) in compare_list:
            for (stu2,file_name2) in compare_list:
                if stu1!=stu2 and isSame(file_name1,file_name2):
                    constant_dict[stu1]*=0
                    constant_dict[stu2]*=0
                    
def CompileAndTest(stu_id,test_id):
    cpp_files=stufile_dict[stu_id][0]
    os.mkdir(getFulldir('test_temp'))
    testdir=test_dir+'/test'+str(test_id)
    os.system("cp -r "+getFulldir(testdir)+" "+getFulldir('test_temp'))
    for file in cpp_files:
        if file[-3:]=='.cpp':
            os.system("cp "+file[0]+" "+getFulldir('test_temp'))
    os.system("cd "+getFulldir('test_temp')+" && g++ -o test "+"code.cpp"+" && ./test")
    os.system("cd .")
    stu_result=None
    for file in cpp_files:
        if file[-10:]=='result.txt':
            stu_result=file[0]
    if isSame(getFulldir('test_temp/test.out'),stu_result):
        score_dict[stu_id]+=10
    if isSame(getFulldir('test_temp/test.out'),getFulldir(testdir+'test_temp/result.txt')):
        score_dict[stu_id]+=testid_scores[test_id]
    os.system("rm -rf "+getFulldir('test_temp'))
    
def test():
    os.system("sudo chmod -R a=rx "+getFulldir(test_dir))
    initScore(20220601,20220612)
    stufile_dict.update(walkDir(test_dir))
    checkSameWithFileName(['code.cpp'])
    for stuid in stufile_dict.keys():
        for testid in range(1,3):
            CompileAndTest(stuid,testid)
    out=open('score.txt','w')
    for stuid in score_dict.keys():
        out.write(stuid+' '+str(score_dict[stuid]*constant_dict[stuid])+'\n')
        
test()