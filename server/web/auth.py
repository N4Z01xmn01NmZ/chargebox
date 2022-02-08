from flask import Blueprint, render_template, request, flash, redirect, url_for
from .db_model import User
from werkzeug.security import generate_password_hash, check_password_hash
from . import db
from flask_login import login_user, login_required, logout_user, current_user

auth = Blueprint('auth', __name__)

@auth.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        email = request.form.get('email')
        password = request.form.get('password')

        user = User.query.filter_by(email=email).first()
        if user:
            if check_password_hash(user.password, password):
                login_user(user, remember=True)
                flash('Logged in successfully', category='success')
                return redirect(url_for('views.home'))
        else:
            flash('User not registered using email: ' + email, category='error')

    return render_template("login.html", user=current_user)

@auth.route('logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('auth.login'))

@auth.route('/sign-up', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        username = request.form.get('username')
        email = request.form.get('email')
        password1 = request.form.get('password1')
        password2 = request.form.get('password2')

        user = User.query.filter_by(email=email).first()
        if user:
            flash('User using email: ' + email + ', already exist', category='error')
        elif len(username) < 4:
            flash('Username is too short.', category='error')
        elif len(email) < 4:
            flash("Email is too short.", category='error')
        elif password1 != password2:
            flash("Password doesn't match.", category='error')
        elif len(password1) < 8:
            flash("Password must be at least 8 characters.", category='error')
        else:
            new_user =  User(email=email, password=generate_password_hash(password1, method='sha256'), username=username)
            db.session.add(new_user)
            db.session.commit()
            login_user(new_user, remember=True)
            flash("Account created: " + username, category='success')
            return redirect(url_for('views.home'))

    return render_template("sign_up.html", user=current_user)
