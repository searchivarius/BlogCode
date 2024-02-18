#!/usr/bin/env python
from time import time

from itertools import chain

REP_QTY = 3
sum_len = 0

for list_len in [1, 10, 100]:
  for rep_list_qty in [10, 100, 1000]:
    lst0 = []
    for k in range(rep_list_qty):
      lst0.append([k] * list_len)
  
    t0 = time()
    for _ in range(REP_QTY):
        lst1 = sum(lst0, [])
        sum_len += len(lst1)
    t1 = time()
    avg_tm = (t1 - t0)/REP_QTY
    print('reg sum time:', avg_tm, 'time / rep. qty:', avg_tm/rep_list_qty, 'total len:', len(lst1), 'time / total len:', avg_tm/len(lst1))

    t0 = time()
    for _ in range(REP_QTY):
        lst1 = list(chain.from_iterable(lst0))
        sum_len += len(lst1)
    t1 = time()
    avg_tm = (t1 - t0)/REP_QTY
    print('itertools time:', avg_tm, 'time / rep. qty:', avg_tm/rep_list_qty, 'total len:', len(lst1), 'time / total len:', avg_tm/len(lst1))
