import subprocess


class Courier:
    def __init__(self, backend):
        self.backend = backend
        self.pipe = subprocess.Popen(backend, bufsize=1024, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    def __del__(self):
        self.write("exit")

    def read(self):
        ret = ''
        # print('read...')
        while True:
            nowline = self.pipe.stdout.readline().decode('UTF-8')
            # print(nowline)
            if nowline == "!!$%!$\n":
                break
            else:
                ret += nowline
        return ret

    def write(self, str):
        # print(str)
        self.pipe.stdin.write((str + '\n').encode('UTF-8'))
        self.pipe.stdin.flush()

    def cmd(self, str):
        self.write(str)
        ret = self.read()
        print("bomb")
        return ret

    def login(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        # print(resultlist)
        return resultlist

    def logout(self, query):
        self.write(query)
        result = self.read()
        # print(ret)

    def add_user(self, query):
        self.write(query)
        result = self.read()
        print(result)
        resultlist = result.split('\n')
        print(resultlist)
        return resultlist

    def query_profile(self, curuserid, userid):
        self.write('query_profile -c ' + curuserid + ' -u ' + userid)
        ret = self.read()
        retList = ret.split('\n')
        if retList[0] == "not_logged" or retList[0] == "privilege_error" or retList[0] == "user_not_found":
            return False, retList[0]
        return True, retList[0].split(' ')

    def modify_profile(self,query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        # print(resultlist)
        return resultlist

    def query_ticket(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        return resultlist

    def query_transfer(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        return resultlist

    def buy_ticket(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        return resultlist

    def query_order(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        return resultlist

    def refund_ticket(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        return resultlist

    def add_train(self, train_str):
        self.write('add_train' + train_str)
        ret = self.read()
        retList = ret.split('\n')
        return retList
        # if retList[0] == "0":
        #     return True, retList[0]
        # return False, retList[0]

    def query_train(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        print(resultlist)
        return resultlist

    def release_train(self, qurey):
        self.write(qurey)
        result = self.read()
        resultlist = result.split('\n')
        return resultlist

    def delete_train(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        return resultlist

    def roll_back(self, query):
        self.write(query)
        result = self.read()
        resultlist = result.split('\n')
        print(resultlist)
        return resultlist
