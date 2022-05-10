### Ticket System Web Front End 开发文档

---

> Author: 杨晋晟
>
> 后续会对此文档进行进一步的修订



#### 任务概述

完成一个与C++车票管理系统对接的Web前端应用，实现所有既有接口，并丰富部分网页端特色功能

应用Client分为普通用户客户端与管理员客户端，分别对应以下操作

![](https://s3.bmp.ovh/imgs/2022/05/10/a2045a0720858bb2.jpg)



#### 技术栈

+ Web Client: Boostrap4 v4.3.1, jQuery v3.2.1
+ Web Server: python v3.7,  flask v1.1.1 (render_template, flask_boostrap)

+ 后端：C++，使用socket与python脚本通信



#### 应用框架

> 注：以下图片均来自早期开发demo，最终产品可能会与之不同

1. Web Client

   + ``/ticketsystem/index``

     应用主入口，火车票管理系统首页

     展示动态、公告、日志等内容，用户可以由首页登录、查看使用指南、查看网站信息等

     ![](https://s3.bmp.ovh/imgs/2022/05/10/7d451689f857f889.jpg)

     ![](https://s3.bmp.ovh/imgs/2022/05/10/9cd349b57a150113.jpg)

     ![](https://s3.bmp.ovh/imgs/2022/05/10/67d1a1ba7cd09d75.jpg)

   + ``/ticketsystem/login``

     登录界面，登陆成功后跳转用户/管理员主页

     <img src="https://s3.bmp.ovh/imgs/2022/05/10/adb93959462db288.jpg" width=500>

   + ``/ticketsystem/user/``，``/ticketsystem/admin/``

     用户主页与管理员主页，通过右侧面板可以进行交易和管理操作

     ![](https://s3.bmp.ovh/imgs/2022/05/10/8a1c87f2f836febc.jpg)

2. Web Server

   使用python+flask框架处理表单请求

   由于使用了boostrap，需要引入render_template和flask_boostrap

   ```python
   from flask import Flask, request, render_template
   from flask_boostrap import Boostrap
   
   app = Flask(__name__)
   boostrap = Boostrap(app)
   
   @app.route('/ticketsystem/index', methods=[GET, POST])
   def index():
       if request.method == "GET":
           return render_template('index.html')
       else: 
           # TO-DO
   
   @app.route('/ticketsystem/user/<id>', methods=[GET, POST])
   def index(id):
       if request.method == "GET":
           return render_template('user_index.html')
       else: 
           # TO-DO
   ```

   

#### 代码框架

+ ``/app/`` 存放web client的html代码与web server的python脚本等
+ ``/app/static/`` 存放javascript代码与css文件
+ ``/app/resource/`` 存放图片等应用资源