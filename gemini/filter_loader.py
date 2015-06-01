#!/usr/bin/env python
#
# Gemini -- Diamond filter loader
#
# Copyright (c) 2012, 2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#
# This preamble is bundled in the filter zipfile as __main__.py.  It creates
# a virtualenv located at /tmp/gemini-$USERID/$(sha1sum file.zip) and
# installs Gemini's Python dependencies.  Finally, it builds the Delphi
# classifier and runs the Diamond filters.
#

from contextlib import closing
import errno
import fcntl
import hashlib
import os
import shutil
import subprocess
import tempfile
import virtualenv
import zipfile

def make_user_tmpdir(system_tmp=None):
    """Create user specific temporary directory
    """
    if system_tmp is None:
        system_tmp = os.environ.get('TMPDIR', '/tmp')
    tmpdir = os.path.join(system_tmp, "gemini-%d" % os.getuid())
    try:
        os.mkdir(tmpdir, 0700)
    except OSError, error:
        if error.errno != errno.EEXIST:
            raise
    return tmpdir

def setup_virtualenv(archive, tmp=None):
    """Setup virtualenv for the requirements.txt embedded in the archive."""

    tmp = make_user_tmpdir(tmp)
    requirements_hash = hashlib.new('sha1',
            archive.read('requirements.txt')).hexdigest()
    venv = os.path.join(tmp, requirements_hash)
    lockfile = venv + '.lock'

    if not os.path.exists(venv):
        with open(lockfile, 'w') as lock:
            fcntl.flock(lock, fcntl.LOCK_EX)
            if not os.path.exists(venv):
                envdir = tempfile.mkdtemp(prefix='venv-', dir=tmp)
                virtualenv.create_environment(envdir, site_packages=True,
                                              never_download=True)
                archive.extract('requirements.txt', envdir)

                REQUIREMENTS = os.path.join(envdir, 'requirements.txt')
                VENV_PIP = os.path.join(envdir, 'bin', 'pip')
                PIPCACHE = os.path.join(os.environ['HOME'], 'pip-cache')
                subprocess.check_call([
                    VENV_PIP, 'install',
                    '--download-cache', PIPCACHE,
                    '--requirement', REQUIREMENTS,
                ], stdout=sys.stderr, stderr=subprocess.STDOUT)

                # link virtualenv into place
                os.chmod(envdir, 0755)
                os.symlink(envdir, venv)

            # venv exists now; safe to delete lock file
            try:
                os.unlink(lockfile)
            except OSError:
                pass

    # activate virtualenv for this process
    activate = os.path.join(venv, 'bin', 'activate_this.py')
    execfile(activate, dict(__file__=activate))
    return venv

# Create PACKAGE specific virtualenv
if __name__ == '__main__':
    import sys

    # initialize virtualenv
    with closing(zipfile.ZipFile(sys.argv[0], 'r')) as archive:
        setup_virtualenv(archive, tmp='/var/tmp')

    from gemini.diamond import build_classifier, run_filters
    build_classifier()
    run_filters()

