from curses import flash
from flask import Blueprint, render_template, request, flash

auth = Blueprint('auth', __name__)

@auth.route('/login', methods=['GET', 'POST'])
def login():
    return render_template("login.html")

@auth.route('logout')
def logout():
    return "<p>Logout</p>"

@auth.route('/sign-up', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        username = request.form.get('username')
        email = request.form.get('email')
        password1 = request.form.get('password1')
        password2 = request.form.get('password2')

        if len(username) < 4:
            flash('Username is too short.', category='error')
        elif len(email) < 4:
            flash("Email is too short.", category='error')
        elif password1 != password2:
            flash("Password doesn't match.", category='error')
        elif len(password1) < 8:
            flash("Password must be at least 8 characters.", category='error')
        else:
            flash("Account created: " + username, category='success')
            # return render_template()

    return render_template("sign_up.html")
