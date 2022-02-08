from enum import Enum
from . import db
from flask_login import UserMixin
from sqlalchemy.sql import func
from enum import Enum
from werkzeug.security import generate_password_hash

class BoxStatus(Enum):
    INACTIVE=0
    VACANT=1
    OCCUPIED=2
    RESERVED=3

class Box(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    uid = db.Column(db.Integer)
    status = db.Column(db.Integer)
    last_access = db.Column(db.DateTime(timezone=True), default=func.now())
    row = db.Column(db.Integer)
    col = db.Column(db.Integer)
    station_id = db.Column(db.Integer, db.ForeignKey('station.id'))
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'))

class Station(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(150))
    boxes = db.relationship('Box')

class User(db.Model, UserMixin):
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(150), unique=True)
    password = db.Column(db.String(150))
    username = db.Column(db.String(150))
    boxes = db.relationship('Box')

def dummy():
    test_user = User(email='testuser@gmail.com', password=generate_password_hash('testpassword', method='sha256'), username='test_user')
    db.session.add(test_user)
    db.session.commit()
    print('Added dummy user')

    test_station = Station(name='testSTA')
    db.session.add(test_station)
    db.session.commit()

    test_user = User.query.filter_by(name='test_user').first()
    test_station = Station.query.filter_by(name='testSTA').first()
    t_box00 = Box(uid=0, status=BoxStatus.INACTIVE, row=0, col=0, station_id=test_station.id)
    db.session.add(t_box00)
    db.session.commit()
    t_box01 = Box(uid=1, status=BoxStatus.INACTIVE, row=0, col=1, station_id=test_station.id)
    db.session.add(t_box01)
    db.session.commit()
    t_box10 = Box(uid=2, status=BoxStatus.OCCUPIED, row=1, col=0, station_id=test_station.id, user_id=test_user.id)
    db.session.add(t_box10)
    db.session.commit()
    t_box11 = Box(uid=3, status=BoxStatus.RESERVED, row=1, col=1, station_id=test_station.id)
    db.session.add(t_box11)
    db.session.commit()
    t_box20 = Box(uid=4, status=BoxStatus.VACANT, row=2, col=0, station_id=test_station.id)
    db.session.add(t_box20)
    db.session.commit()
    t_box21 = Box(uid=5, status=BoxStatus.VACANT, row=2, col=1, station_id=test_station.id)
    db.session.add(t_box21)
    db.session.commit()

