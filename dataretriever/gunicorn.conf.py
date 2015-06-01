# gunicorn config file

bind = '0.0.0.0:8026'
workers = 1
worker_class = 'gevent'
pidfile = 'gunicorn.pid'
proc_name = 'dermshare_dataretriever'
