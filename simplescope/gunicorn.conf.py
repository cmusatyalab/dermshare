# gunicorn config file

bind = '0.0.0.0:8025'
workers = 2
worker_class = 'gevent'
pidfile = 'gunicorn.pid'
proc_name = 'simplescope'
