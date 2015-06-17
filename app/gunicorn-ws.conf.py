# gunicorn config file for dermshare-ws.py

bind = '0.0.0.0:8026'
# Only one worker, since connections need to share state
workers = 1
worker_class = 'geventwebsocket.gunicorn.workers.GeventWebSocketWorker'
pidfile = 'gunicorn-ws.pid'
proc_name = 'dermshare-ws'
