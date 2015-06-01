#
# Gemini -- machine learners
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

import numpy as np
from sklearn import ensemble
from sklearn import cross_validation
from sklearn.preprocessing import MinMaxScaler as Scaler

def trainer(X_train, y_train):
    clf = ensemble.ExtraTreesClassifier(
        bootstrap=False, compute_importances=None,
        criterion='gini', max_depth=None, max_features=None,
        min_density=None, min_samples_leaf=1, min_samples_split=2,
        n_estimators=1000, oob_score=False, random_state=0)
    clf.fit(X_train, y_train)
    return clf

def tester(clf, X_train, y_train):
    y_train = np.asarray(y_train)
    scores = cross_validation.cross_val_score(clf, X_train, y_train, cv=5)
    return scores.mean(), scores.std() * 2

