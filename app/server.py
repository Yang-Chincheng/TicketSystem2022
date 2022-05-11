from flask import Flask, render_template, request
from flask_boostrap import Boostrap

app = Flask(__name__)
boostrap = Boostrap(app)