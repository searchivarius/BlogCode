## Overview
A summary of results for QA and MNLI case. The models were trained on G5 AWS instances using different subses. A small subset had 4K entries and the large one had at most 40K. We tried both FP16 and BF16 float-poing modes, but they performed similarly. Parallel training was carried out using HuggigngFace accelerate library extended to support LocalSGD. As a baseline we use single-GPU training and training with gradient accumulation (which makes synchronization steps less frequent, but increases an effective batch size). Each model is trained three times using different seeds.

## Tables and plots
If you click on the image, you can download Excel files with more detailed tables. Note that for XNLI we remove a few datapoints where performance was too low. 

[![QA 4K](QA_small.png)](QA_small.xls) [![QA 40K](QA_large.png)](QA_large.xls)

[![MNLI 4K](MNLI_small.png)](MNLI_small.xls) [![MNLI 40K](MNLI_large.png)](MNLI_large.xls)

## A brief discussion of results

It is not hard to see the following:
1. There is nearly always at least a small degradation when a model is trained with multiple GPUs.
2. Unfortunately, training with 8 GPUs produces less accurate models than training with 4 GPUs.
3. Yet, LocalSGD with 4 GPUs can nearly **maximize the speed up** while remaining **nearly as accurate** as the 1 GPU baseline.
4. For larger datasets, a degradation is less than for smaller ones.
5. Performance of LocalSGD is much more robust with respect achievable speed up.
6. In particular, LocalSGD achieves better accuracy when we aim to achieve a speed up close to the numer of available GPUs.
7. Last, but not least, there is some instability in training (see detailed tables). This is why we train with three seeds and record both maximium and average values.
