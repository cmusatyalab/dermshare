# gunicorn config file for dermshare.py

bind = '0.0.0.0:8024'
workers = 8
worker_class = 'gevent'
pidfile = 'gunicorn.pid'
proc_name = 'dermshare'
