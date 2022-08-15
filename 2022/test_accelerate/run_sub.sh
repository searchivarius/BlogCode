#!/bin/bash -e

set -o pipefail

conda activate test_accelerate

export CUDA_VERSION=11.3
conda install pytorch cudatoolkit=$CUDA_VERSION -c pytorch -y

pip install "transformers==4.21.1" 
pip install "datasets"
pip install "wrapt" "flatbuffers"
pip install "huggingface_hub"
pip install "accelerate"

echo "Installation finished!"

BERT_MODEL=bert-large-uncased
BATCH_SIZE=8

LR=3e-5

for MAX_TRAIN_SAMPLES in 50000 5000 ; do
    OUTPUT_PREF=output_res_${MAX_TRAIN_SAMPLES}

for SEED in 0 1 2 ; do

    # These runs for non-synchronous gradient descent
    for no_sync_steps in 1 2 4 8 16 ; do
        out_dir=${OUTPUT_PREF}_nosync_steps_${no_sync_steps}/$SEED
        rm -f $out_dir
        mkdir -p $out_dir

        # The first run is on a single GPU and without gradient accumulation
        accelerate launch run_qa_no_trainer_nonsync.py \
          --max_train_samples $MAX_TRAIN_SAMPLES \
          --no_sync_steps $no_sync_steps \
          --model_name_or_path bert-large-uncased \
          --per_device_train_batch_size $BATCH_SIZE \
          --learning_rate $LR \
          --seed $SEED \
          --dataset_name squad \
          --max_seq_length 384 \
          --doc_stride 128  \
          --output_dir $out_dir  2>&1|tee $out_dir/run.log
    done

    out_dir=${OUTPUT_PREF}_1gpu/$SEED/
    rm -f $out_dir
    mkdir -p $out_dir
    python run_qa_no_trainer.py \
      --max_train_samples $MAX_TRAIN_SAMPLES \
      --model_name_or_path bert-large-uncased \
      --per_device_train_batch_size $BATCH_SIZE \
      --gradient_accumulation_steps 1 \
      --learning_rate $LR \
      --seed $SEED \
      --dataset_name squad \
      --max_seq_length 384 \
      --doc_stride 128  \
      --output_dir $out_dir  2>&1|tee $out_dir/run.log

    for grad_accum_steps in 1 2 4 8 16 ; do
        output_dir=${OUTPUT_PREF}_accum_steps_${grad_accum_steps}/$SEED/
        rm -f $out_dir
        mkdir -p $out_dir
        accelerate launch  run_qa_no_trainer.py \
          --max_train_samples $MAX_TRAIN_SAMPLES \
          --model_name_or_path bert-large-uncased \
          --per_device_train_batch_size $BATCH_SIZE \
          --gradient_accumulation_steps $grad_accum_steps \
          --learning_rate $LR \
          --seed $SEED \
          --dataset_name squad \
          --max_seq_length 384 \
          --doc_stride 128  \
          --output_dir $out_dir  2>&1|tee $out_dir/run.log
    done
    
done

done
