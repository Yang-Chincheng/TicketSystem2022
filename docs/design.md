## Train Ticket Management System 开发文档

> written by 杨晋晟, 陈永杉



### 项目概述

- 实现一个类似于 [12306](https://www.12306.cn/) 的火车票订票系统，该系统向用户提供购票业务相关功能，包括车票查询、购票、订单操作等，以及向管理员提供后台管理功能

- 系统需将用户数据、购票数据、车次数据进行**本地存储**，并对其进行高效操作

  

### 数据存储

#### 存储模块

##### 1. 内存池

实现外存空间的回收；使用vector类实现数据缓存

```c++
template<class value_type>
class MemoryPool {
private:
    int elements_num_;
    vector<value_type> storage_;
    std::fstream pool_;
    std::string pool_name_;
    
public:
    MemoryPool(const std::string &name_in);
    ~MemoryPool();
    void add(const value_type &value_in);
    value_type getback();
    bool empty();
    void clear();
    size_t size();
};
```

##### 2. B+树

用于在外存上存储需要随机访问的有序数据

```c++
template<class key_type, class value_type>
class BPTree {
private:
    struct key_address_pair{}
    class Node{
        key_address_pair data[k_max_size + k_min_size];
        int prev_num = -1;
        int next_num = -1;
        int my_num = -1;
        int parent_num = -1;
        int elements_num = 0;
        bool if_leaf = true;
        Node();
        ~Node();
    };
    int node_num_; // 所有已经开过的块数量，只增不减
    int data_num_; // 所有已经开过的空间数量，只增不减
    int record_num_; // 内部存储的记录数量
    int height_; // B+树的高度
    std::string index_name_;
    std::string data_name_;
    std::fstream index_;
    std::fstream data_;
    Node_ root_;
    MemoryPool<int> node_memory_pool_;
    MemoryPool<value_type> data_memory_pool_;
    
public:
    BPTree(const std::string &name_in); 
    ~BPTree();
    bool empty();
    void clear();
    size_t size();
    int height();
    bool search(const key_type &key);
    Node get(const key_type &key);
    void insert(const key_type &key_in, const value_type &value_in);
    void erase(const key_type &key_in, const value_type &value_in);
    void erase(const key_type &key_in);
private:
    void NodeCopy_(Node_ &old_node_in, Node_ &new_node_in, int start_pos = 0, bool if_half = false);
    void DataMerge_(Node_ &first_node_in, Node_ &second_node_in);
    void BreakNode_(Node_ node_in);
    void DeleteFix_(Node_ object_node, key_type deleted_key);// 如果删除了块的第一个元素，则其parent的节点也要相应的变化
    void MergeNode_(Node_ node_in);
    Node GetFirstNode_();
    Node GetLastNode_();
};
```

##### 3. 十字链表

用于在外存上存储需要顺序访问的双向有序数据

```c++
template <class value_type>
class OrthogonalList {
private:
    struct Node {
        value_type data;
        int next_x, prev_x;
        int next_y, prev_y;   
        Node();
        ~Node();
    };
    size_t node_num_;
    size_t record_num_;
    int x_head_;
    int y_head_;
    MemoryPool<Node> node_memory_pool_;
    
public:
    OrthogonalList();
    ~OrthogonalList();
    void insert(value_type data_in);
    Node* find(value_type data_in);
    void erase(value_type data_in);
    bool empty();
    void clear();
    size_t size();
};
```

#### 其他数据结构

##### 1. 线段树

主要用于维护余票信息，支持区间add、查询区间min

```c++
template <size_t len> struct Segtree;
```

##### 2. 哈希表

主要用于维护登录信息，支持随机访问、赋值

```c++
template <typename _Key, typename _Tp> struct Hashmap;
```



### 主体逻辑

#### 工具

##### 1. 字符串

封装一个定长的C风格字符串，并丰富其功能

```c++
template <size_t maxlen>
class Str {
private:
    char str[maxlen];
public:
    Str();
    Str(const char *_str);
    Str(const std::string &_str);
    template <size_t len> Str(const Str<len> *_str);
    ~Str();
    // 类型转换
    operator const char*() const;
    operator std::string() const;
    // 随机访问
    char& operator [] (size_t idx);
    const char& operator [] (size_t idx) const;
    // 输入输出
    friend std::istream &operator >> (std::istream &is, Str &obj);
    friend std::ostream &operator << (std::ostream &os, const Str &obj);
    // 一系列字典序比较函数，以 == 为例
    template <size_t len1, size_t len2>
    friend bool operator == (const Str<len1> &lhs, const Str<len2> &rhs);
    // 判断该字符串是否合法，在派生类中重写
    bool isvalid() const {return 1; }
};

class Username: public Str<usr_len>;
class Password: public Str<psw_len>;
class Name: public Str<name_len>;
class MailAddr:public Str<maddr_len>;

class TrainID: public Str<trID_len>;
class Station: public Str<sta_len>;

class Command: public Str<cmd_len>;
```

##### 2. Token Scanner

用于解析命令行输入；处理类型转换

```c++
class TokenScanner {
private:
    std::string buff;
public:
    TokenScanner();
    ~TokenScanner();
    TokenScanner(const std::string& _buff);
    template <typename T> T next_token();
};
```

##### 3. 时间类与日期类

处理时间与日期的判断、换算等

```c++
struct Date {
	int month, day;
    Date(int mn, int dy);
    ~Date();
    Date& operator += (int days);
    Date& operator -= (int days);
    operator std::string () const;
    friend bool operator < (const Date &lhs, const Date &rhs);
    // 相差多少天
    firend int operator - (const Date &lhs, const Date &rhs);
};
struct Time {
    Date date;
    int hour, min;
    Time(int mn, int dy, int hr, int mi);
    ~Time();
    operator std::string() const;
    Time& operator += (int mins);
    Time& operator -= (int mins);
    friend bool operator < (const Time &lhs, const Time &rhs);
    // 相差多少分钟
    friend int operator - (const Time &lhs, const Time &rhs);
};
```

##### 5. 异常处理

```c++
class Exception {
    std::string msg;
public:
    Exception(const std::string &_msg);
    std::string what() const;
};
class Exeption1: public Exception ...
```

##### 6. 其他

实现于 ``utility.hpp``，包括 ``mergesort`` 、``pair`` 等杂项



#### 用户管理

##### 1. 用户信息

记录用户的名称、密码等信息，使用B+树存储

```c++
// 除username外的用户信息
struct UserInfo {
    Password pwd; // 密码
    Name name; // 姓名
    MailAddr maddr; // 邮件地址
    int pri; // 优先级
    UserInfo(args...);
    ~UserInfo();
    friend std::ostream& operator << (std::ostream &os, const UserInfo &us);
};
```

##### 2. 登录状态

使用内存中的哈希表来维护用户的登录状态

```c++
Hashmap<Username, bool> online;
```

##### 3. 用户管理

用户管理类，实现接口

```c++
class UserManager {
private:
    BPTree<Username, UserInfo> user;
    Hashmap <Username, bool> online;
public:
    int add_user(args...);
   	int login(const Username &u, const Password &p);
    int logout(const Username &u);
    int query_profile(const Username &u, const Username &u);
    int modify_profile(args...);
    void clear();
};
```



#### 列车管理

##### 1. 列车信息

记录每一辆列车的站点、用时、余票等信息，使用B+树存储

```c++
// 除trainID外的列车信息
struct TrainInfo {
    bool released; // 是否已发布
    char type; // 列车类型
    int station_num; // 经过的车站数量
    Station sta[max_stanum]; // 经过的车站名
    int price[max_stanum]; // 两站之间的票价
    int travel_time[max_stanum]; // 两站之间的用时
    int stop_time[max_stanum]; // 每一站的停留时间
    Time start_time; // 每日发车时间
   	Date start_date, end_date; // 售卖时间区间
    Segtree<max_stanum> seat[max_date]; // 每一车次的余票量
    
    TrainInfo(args...);
    ~TrainInfo();
    friend std::ostream& operator << (std::ostream &os, const TrainInfo& tr);
};
```

##### 2. 站点信息

对于每一个站点，记录所有经过它的**已发布**的列车的信息摘要，以及完整信息链接

```c++
struct MetaData {
    Date start_date, end_date; // 列车信息摘要
	int pos; // 完整列车信息的offset
};
struct PassTrain {
	int num; // 经过的列车数
    MetaData pass_train[max_passnum];
};
```

##### 3. 列车管理

列车管理类，提供接口

```c++
class TrainManager {
private:
    BPTree<TrainID, TrainInfo> train;
    BPTree<Station, PassTrain> station;
public:
    TrainManager();
    ~TrainManager();
	
    int rest_ticket(TrainID &id, Date &d, Station &s, Station &t);
    
    int add_train(args...);
    int release_train(const TrainID &id);
    int undo_release(const TrainID &id); // 用于rollback
    int query_train(const TrainID &id, const Date &date);
    
    template <typename _Cmp>
    int query_ticket(Station &s, Station &t, Date &d, _Cmp &cmp);
    template <typename _Cmp>
    int query_transfer(Station &s, Station &t, Date &d, _Cmp &cmp);
    
    // 查询列车id在日期d从s站出发并最终到达t站的余票量&单价
    // 需要从日期d以及出发站s推断列车的始发站发车日期
    pair<int, int> qeury_seat(TrainID &id, Station &s, Station &t, Date &d);
    // 更改在日期d出发的列车id从s站到t站的余票量
    int modify_seat(TrainID &id, Station &s, Station &t, Date &d, int delta);
    void clear();
};
```



#### 交易管理

##### 1. 交易记录

使用十字链表存储，x方向记录用户的每一条交易，y方向记录列车对应的每一条交易

```c++
enum Status {SUCCESS, PENDING, REFUNDED};
struct TraxInfo {
	Status staus; // 交易状态
    TrainID id; // 列车id
    Station from; // 出发站点
    Station to; // 到达站点
    Time leaving_time; // 出发时间
    Time arrving_time; // 到达时间
    int price; // 票价
    int num; // 购票量
    
    TraxInfo(args...);
    ~TraxInfo();
    firend std::ostream& operator << (std::ostream &os, const TraxInfo &o);
};
```

##### 2. 交易管理

```c++
class TraxManager {
private:
	OrthogonalList<TraxInfo> trax;
public:
    TraxManager();
    ~TraxManager();
    
    // 在相应位置插入一条记录
    int insert_record(TraxInfo &rec);
    // 删除某个用户的最后一条交易记录，用于rollback
    int erase_record(Username &user);
    // 更改某条交易记录的status
    int update_record(TraxInfo &rec, Status new_status);
    void clear();
    
};
```



#### 操作日志

用于回滚指令，对于每一条操作，记录其回滚所需的操作

```c++
struct Log {
	int idx; // 时间戳
    std::string opt; // 回滚所需的操作及其信息
};
```

操作日志在外存中顺序存储

```c++
class LogManager {
private:
	MemoryRiver<Log> log;
public:
    LogManager();
    ~LogManager();
    // 写入一条操作日志
    void add_log(int time, std::string opt);
    // 根据操作日志回滚一次，返回当前所在时刻
	int rollback();
};
```



#### 系统管理

火车票管理系统的最外层封装，实现指令接口

```c++
struct SysManager {
	UserManager user;
    TrainManager train;
    TraxManager trax;
    LogManager log;
    
    SysManager();
    ~SysManager();
    
    // 系统接口
    int Main(std::string &cmd);
    int add_user(...);
    int login(...);
    int logout(...);
    int query_profile(...);
    int modify_profile(...);
    int add_train(...);
    int release_train(...);
    int query_train(...);
    int query_ticket(...);
    int query_transfer(...);
    int buy_ticket(...);
    int query_order(...);
    int refund_ticket(...);
    int rollback(...);
    int clear();
    int exit();
    
};
```



### 文件结构

#### 1. 数据及索引文件

+ ``train_index_storage.dat``，存储列车索引信息
+ ``train_index_memorypool_storage.dat``，存储列车索引信息回收的外存空间
+ ``train_data_storage.dat``，存储列车数据信息
+ ``train_data_memorypool_storage.dat``，存储列车数据信息回收的外存空间
+ ``user_index_storage.dat``，存储用户索引信息
+ ``user_index_memorypool_storage.dat``，存储用户索引信息回收的外存空间
+ ``user_data_storage.dat``，存储用户数据信息
+ ``user_data_memorypool_storage.dat``，存储用户数据信息回收的外存空间
+ ``transaction.dat``，存储交易信息
+ ``log.dat``，存储操作日志

#### 2. 代码文件

+ **数据存储**

  ``bptree.h``：实现B+树，包含 ``BPTree`` 类等

  ``orthlist.h``：实现十字链表，包含 ``OrthogonalList`` 类等

  ``hashmap.h``：实现哈希表，包含 ``Hashmap`` 类等

+ **工具**

  ``str.h``：实现字符串处理，包含 ``Str`` 类、``TokenScanner`` 类等

  ``utility.h``：杂项，包含 ``pair`` 类、``Date`` 类、``Time`` 类、``mergesort`` 函数等

  ``exception.h``：异常处理

+ **主体逻辑**

  ``user.h/cpp``：实现用户管理，包含 ``UserInfo`` 类、``UserManager`` 类等

  ``train.h/cpp``：实现列车管理，包含 ``TrainInfo`` 类、``TrainManager`` 类等

  ``transaction.h/cpp``：实现交易管理，包含 ``TraxInfo`` 类、``TraxManager`` 类等

  ``log.h/cpp``：实现日志管理，包含 ``LogInfo`` 类、``LogManager`` 类等

  ``sys.h/cpp``：实现系统管理，包含 ``SysManager`` 类等

  ``main.cpp``：程序主入口，解析cmd输入并调用相应函数