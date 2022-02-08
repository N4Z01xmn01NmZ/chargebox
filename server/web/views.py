from urllib import request
from flask import Blueprint, render_template, request, flash, redirect, url_for
from flask_login import login_required, current_user

views = Blueprint('views', __name__)

@views.route('/', methods=['GET', 'POST'])
@login_required
def home():
    if request.method == 'POST':
        print(request.form)
    return render_template("home.html", user=current_user)

@views.route('/sta', methods=['GET'])
@login_required
def station():
    return render_template("station.html")

@views.route('uid-gen')
@login_required
def uid_gen():
    return render_template("uid_gen.html", user=current_user)
