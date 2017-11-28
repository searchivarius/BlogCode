# The code is based on the following blog-post
# https://gurus.pyimagesearch.com/lesson-sample-k-nearest-neighbor-classification/
# import the necessary packages
from __future__ import print_function
from sklearn.cross_validation import train_test_split
from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import classification_report
from sklearn import datasets
from sklearn.datasets import fetch_mldata
import numpy as np
import imutils
import sys
#from skimage import exposure
import cv2
 
# load the MNIST digits dataset
#mnist = datasets.load_digits()
mnist = fetch_mldata('MNIST original')

print("Total # of data points %d" % (len(mnist.data)))
 
# take the MNIST data and construct the training and testing split, using 75% of the
# data for training and 25% for testing
(trainData0, testData, trainLabels0, testLabels) = train_test_split(np.array(mnist.data),
	mnist.target, test_size=0.03, random_state=42)
 
# now, let's take 10% of the training data and use that for validation
(trainData0, valData, trainLabels0, valLabels) = train_test_split(trainData0, trainLabels0,
	test_size=0.03, random_state=84)

TRAIN_QTY = 200
uniqLabs = set(trainLabels0)
qtys = dict( (k, 0) for k in uniqLabs)

trainData = []
trainLabels = []

totQty = 0
for i in range(0, len(trainLabels0)):
  lab = trainLabels0[i]
  if qtys[lab] < TRAIN_QTY:
    qtys[lab] = qtys[lab] + 1
    trainData.append(trainData0[i])
    trainLabels.append(trainLabels0[i])
    totQty = totQty + 1
    if totQty >= TRAIN_QTY * len(uniqLabs):
      break

trainData = np.array(trainData)
trainLabels = np.array(trainLabels)

print("# of training samples used %d" % (len(trainData)))

# show the sizes of each data split
print("training data points: {}".format(len(trainLabels)))
print("validation data points: {}".format(len(valLabels)))
print("testing data points: {}".format(len(testLabels)))

# initialize the values of k for our k-Nearest Neighbor classifier along with the
# list of accuracies for each value of k
kVals = range(1, 30, 2)
accuracies = []
 
# loop over various values of `k` for the k-Nearest Neighbor classifier
for k in xrange(1, 30, 2):
	# train the k-Nearest Neighbor classifier with the current value of `k`
	model = KNeighborsClassifier(n_neighbors=k)
	model.fit(trainData, trainLabels)
 
	# evaluate the model and update the accuracies list
	score = model.score(valData, valLabels)
	print("k=%d, accuracy=%.2f%%" % (k, score * 100))
	accuracies.append(score)
 
# find the value of k that has the largest accuracy
i = np.argmax(accuracies)
print("k=%d achieved highest accuracy of %.2f%% on validation data" % (kVals[i],
	accuracies[i] * 100))
