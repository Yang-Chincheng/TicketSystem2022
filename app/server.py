from flask import Flask, render_template, request
from Cour import Courier
from flask_bootstrap import Bootstrap
import json
# import subprocess
import time

app = Flask(__name__)

boostrap = Bootstrap(app)

# TestInfo = {}

courier = Courier('./code')


def connect_strlist(strlist):
    if len(strlist) == 0:
        return '_'
    ret = str()
    for s in strlist:
        ret += s + '|'
    return ret[0: len(ret) - 1]


user_now = ""


@app.route("/index", methods=['GET', 'POST'])
def index():
    return render_template('index.html')


@app.route('/login', methods=['GET', 'POST'])
def login():
    global user_now
    if request.method == "GET":
        return render_template('login.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        user_name = request.form.get("user_name")
        user_password = request.form.get("user_password")
        query = '[{}] login -u {} -p {}'.format(str(time_now), user_name, user_password)
        output = courier.login(query)
        row = output[0].split()
        if row[1] == '-1':
            return json.dumps({"code": row[1], "data": ""})
        else:
            user_now=user_name
            return json.dumps({"code": "0", "data": user_name})


@app.route('/user/index', methods=['GET', 'POST'])
def user_index():
    return render_template('user/index.html',username = user_now)


@app.route('/user/buy-ticket', methods=['GET', 'POST'])
def user_buyticket():
    global user_now
    if request.method == "GET":
        return render_template('user/buy-ticket.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        id = request.form.get("id")
        date = request.form.get("date")
        start = request.form.get("start")
        end = request.form.get("end")
        num = request.form.get("num")
        pending = request.form.get("pending")
        query = "[{}] buy_ticket -u {} -i {} -d {} -n {} -f {} -t {} -q {}".format(time_now, user_now, id, date, num,
                                                                                   start, end, pending)
        output = courier.buy_ticket(query)

        print('[checkbox]', pending)
        print('>>>', query)
        print('<<<', output)

        first_row = output[0].split()
        if first_row[1] == '-1':
            info = {"code": first_row[1], 'data': ""}
            # print(info)
            return json.dumps(info)
        else:
            info = {"code": '0', "data": first_row[1]}
            return json.dumps(info)

@app.route('/user/query-order', methods=['GEt', 'POST'])
def user_queryorder():
    global user_now
    if request.method == "GET":
        return render_template('user/query-order.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        query = "[{}] query_order -u {}".format(time_now, user_now)
        print(query)
        output = courier.query_order(query)

        print(">>>", output)

        first_row = output[0].split()
        num = int(first_row[1])
        if num == -1:
            return {"code": '-1', 'data': ""}
        else:
            orders = []
            for i in range(1, len(output) - 1):
                row = output[i].split()
                row_1 = {"status": row[0], "id": row[1], "from": row[2], "leave": "{} {}".format(row[3], row[4]),
                         "to": row[6], "arrive": "{} {}".format(row[7], row[8]), "price": row[9], "ticket": row[10]}
                orders.append(row_1)
                print(row_1)
            # print(trains)
            info = {
                "code": '0', "data": {
                    "num": num,
                    "trains": orders
                }
            }
            print(">>", info)
            return json.dumps(info)


@app.route('/user/query-train', methods=['GEt', 'POST'])
def user_querytrain():
    global user_now
    if request.method == "GET":
        return render_template('user/query-train.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        train_id = request.form.get("id")
        train_data = request.form.get("date")
        query = '[{}] query_train -i {} -d {}'.format(str(time_now), train_id, train_data)
        print(query)
        output = courier.query_train(query)
        first_row = output[0].split()
        if first_row[1] == '-1':
            # print('fuck\n')
            info = {"code": first_row[1], 'data': ""}
            print(info)
            return json.dumps(info)
        else:
            stations = []
            for i in range(1, len(output) - 1):
                row = output[i].split()
                print(row)
                row_1 = {"name": row[0], "arrive": "{} {}".format(row[1], row[2]),
                         "leave": "{} {}".format(row[4], row[5]), "price": row[6], "ticket": row[7]}
                stations.append(row_1)
            print(stations)
            info = {
                "code": '0', "data": {
                    "id": first_row[1],
                    "type": first_row[2],
                    "stations": stations
                }
            }
            return json.dumps(info)


@app.route('/user/query-ticket', methods=['GEt', 'POST'])
def user_queryticket():
    global user_now
    if request.method == "GET":
        return render_template('user/query-ticket.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        start = request.form.get("start")
        to = request.form.get("to")
        date = request.form.get("date")
        appendix = request.form.get("appendix")
        type = request.form.get("type")
        if type == 'ticket':
            query = "[{}] query_ticket -s {} -t {} -d {} -p {}".format(str(time_now), start, to, date, appendix)
            output = courier.query_ticket(query)
            first_row = output[0].split()
            num = int(first_row[1])
            trains = []
            for i in range(1, len(output) - 1):
                row = output[i].split()
                row_1 = {"id": row[0], "from": row[1], "leave": "{} {}".format(row[2], row[3]), "to": row[5],
                         "arrive": "{} {}".format(row[6], row[7]), "price": row[8], "ticket": row[9]}
                trains.append(row_1)
            # print(trains)
            info = {
                "code": '0', "data": {
                    "num": num,
                    "trains": trains
                }
            }
            return json.dumps(info)
        else:
            query = "[{}] query_transfer -s {} -t {} -d {} -p {}".format(str(time_now), start, to, date, appendix)
            output = courier.query_transfer(query)

            print(">>>", query)
            print("<<<", output)

            first_row = output[0].split()
            if first_row[1]=='0':
                return json.dumps({"code": '-1', 'data': ""})
            else:
                trains = []
                row = output[0].split()
                row_1 = {"id": row[1], "from": row[2], "leave": "{} {}".format(row[3], row[4]), "to": row[6],
                             "arrive": "{} {}".format(row[7], row[8]), "price": row[9], "ticket": row[10]}
                trains.append(row_1)
                row = output[1].split()
                row_1 = {"id": row[0], "from": row[1], "leave": "{} {}".format(row[2], row[3]), "to": row[5],
                             "arrive": "{} {}".format(row[6], row[7]), "price": row[8], "ticket": row[9]}
                trains.append(row_1)
                # print(trains)
                info = {
                    "code": '0', "data": {
                        "trains": trains
                    }
                }
                return json.dumps(info)

@app.route('/user/refund-ticket', methods=['GEt', 'POST'])
def user_refundticket():
    global user_now
    if request.method == "GET":
        return render_template('user/refund-ticket.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        num = request.form.get("num")
        query = "[{}] refund_ticket -u {} -n {}".format(time_now, user_now, num)
        output = courier.refund_ticket(query)

        print(">>>", query)
        print("<<<", output)

        first_row = output[0].split()
        info = {"code": first_row[1], 'data': ""}
        print(">>>", info)
        return json.dumps(info)


@app.route('/admin/add-train', methods=['GEt', 'POST'])
def admin_addtrain():
    global user_now
    if request.method == "POST":
        if request.method == "POST":
            time_now = time.strftime("%m%d%H%M%S", time.localtime())
            train_id = request.form.get("id")
            train_type = request.form.get("type")
            train_time = request.form.get("time")
            train_start_day = request.form.get("start")
            train_end_day = request.form.get("end")
            train_seat_num = request.form.get("seatnum")
            train_station_num = int(request.form.get("stanum"))
            station = request.form.getlist("station[]")
            travel = request.form.getlist("travel[]")
            stop = request.form.getlist("stop[]")
            price = request.form.getlist("price[]")

            station_name = connect_strlist(station)
            travel_times = connect_strlist(travel[0: train_station_num - 1])
            stop_overtimes = connect_strlist(stop[0: train_station_num - 2])
            prices = connect_strlist(price[0: train_station_num - 1])

            query = f'[{time_now}] add_train -i {train_id} -n {train_station_num} -m {train_seat_num} '
            query += f'-s {station_name} -p {prices} -x {train_time} -t {travel_times} '
            query += f'-o {stop_overtimes} -d {train_start_day}|{train_end_day} -y {train_type}'
            print(query)
            output_raw = courier.add_train(query)[0]
            output = output_raw.split()
            print(output)
            ans = int(output[1])
            return json.dumps({"code": ans, "data": ""})

    if request.method == "GET":
        return render_template('admin/add-train.html')


@app.route('/admin/add-user', methods=['GEt', 'POST'])
def admin_adduser():
    global user_now
    if request.method == "GET":
        return render_template('admin/add-user.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        id = request.form.get("id")
        mail = request.form.get("mail")
        name = request.form.get("name")
        password = request.form.get("password")
        pri = request.form.get("pri")
        query = "[{}] add_user -c {} -u {} -p {} -n {} -m {} -g {}".format(time_now, user_now, id, password, name, mail,
                                                                           pri)
        print(query)
        output = courier.add_user(query)
        first_row = output[0].split()
        return json.dumps({"code": first_row[1], "data": ""})


@app.route('/admin/clean', methods=['GEt', 'POST'])
def admin_clean():
    global user_now
    if request.method == "GET":
        return render_template('admin/clean.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        type = request.form.get("type")
        query = str()
        if type == "clean":
            query = "[{}] clean".format(time_now)
        if type == "exit":
            query = "[{}] exit".format(time_now)
        output = courier.handle(query)
        first_row = output[0].split()
        return json.dumps({"code": first_row[1], "data": ""})


@app.route('/admin/delete-train', methods=['GEt', 'POST'])
def admin_deletetrain():
    global user_now
    if request.method == "GET":
        return render_template('admin/delete-train.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        id = request.form.get("id")
        query = "[{}] delete_train -i {}".format(time_now, id)
        output = courier.delete_train(query)
        first_row = output[0].split()
        return json.dumps({"code": first_row[1], "data": ""})

@app.route('/admin/index', methods=['GEt', 'POST'])
def admin_index():
    return render_template('admin/index.html', username = user_now)


@app.route('/admin/modify-profile', methods=['GEt', 'POST'])
def admin_modifyprofile():
    global user_now
    if request.method == "GET":
        return render_template('admin/modify-profile.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        id = request.form.get("id")
        mail = request.form.get("mail")
        name = request.form.get("name")
        password = request.form.get("password")
        pri = request.form.get("pri")
        query = "[{}] modify_profile -c {} -u {}".format(time_now, user_now, id)
        if len(password) != 0:
            query += " -p {}".format(password)
        if len(name) != 0:
            query += " -n {}".format(name)
        if len(mail) != 0:
            query += " -m {}".format(mail)
        if len(pri) != 0:
            query += " -g {}".format(pri)
        output = courier.modify_profile(query)
        first_row = output[0].split()
        if first_row[1] == '-1':
            return json.dumps({"code": first_row[1], "data": ""})
        else:
            return json.dumps({"code": '0', "data": {"id": first_row[1], "name": first_row[2], "mail": first_row[3],
                                                     "pri": first_row[4]}})


@app.route('/admin/query-user', methods=['GEt', 'POST'])
def admin_queryuser():
    global user_now
    if request.method == "GET":
        return render_template('admin/query-user.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        id = request.form.get("id")
        query = "[{}] query_profile -c {} -u {}".format(time_now, user_now, id)
        output = courier.modify_profile(query)

        print('>>>', query)
        print('<<<', output)

        first_row = output[0].split()

        print('fr', output[0], first_row)

        if first_row[1] == '-1':
            return json.dumps({"code": first_row[1], "data": ""})
        else:
            return json.dumps({"code": '0', "data": {"id": first_row[1], "name": first_row[2], "mail": first_row[3],
                                                     "pri": first_row[4]}})


@app.route('/admin/release-train', methods=['GEt', 'POST'])
def admin_releasetrain():
    global user_now
    if request.method == "GET":
        return render_template('admin/release-train.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        id = request.form.get("id")
        query = "[{}] release_train -i {}".format(time_now, id)
        output = courier.release_train(query)
        first_row = output[0].split()
        return json.dumps({"code": first_row[1], "data": ""})


@app.route('/admin/rollback', methods=['GEt', 'POST'])
def admin_rollback():
    global user_now
    if request.method == "GET":
        return render_template('admin/rollback.html')
    if request.method == "POST":
        time_now = time.strftime("%m%d%H%M%S", time.localtime())
        obj_time = request.form.get("time")
        query = "[{}] rollback -t {}".format(time_now, obj_time)
        output = courier.roll_back(query)
        first_row = output[0].split()
        if first_row[1] == '0':
            user_now = str()
        return json.dumps({"code": first_row[1], "data": ""})

@app.route('/')
def notfound():
    return '223423 404 NOT Found'


if __name__ == '__main__':
    query = '[0] add_user -c _ -u __Schale__ -p DECAGRAMMATON -n sensei -m a@b -g 20'
    courier.add_user(query)
    # query = '[0] login -u __Schale__ -p DECAGRAMMATON'
    # courier.login(query)
    app.run(host='0.0.0.0',
            port=7777,
            # debug=true
            )
