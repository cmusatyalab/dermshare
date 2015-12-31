#
# DermShare Autosegmenter
#
# Copyright (c) 2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT

from contextlib import contextmanager
import numpy as np
import random
import signal

@contextmanager
def watchdog(timeout):
    try:
        signal.alarm(timeout)
        yield
    finally:
        signal.alarm(0)

#class watchdog(object):
#    def __init__(self, timeout):
#        self.timeout = timeout
#
#    def __enter__(self):
#        signal.alarm(self.timeout)
#
#    def __exit__(self, exc_type, exc_val, exc_tb):
#        signal.alarm(0)
#        return False

def make_boring():
    """ try to make random number generation predicable """
    random.seed(0)
    np.random.seed(0)

