# Orbit Integration

Planned to use galpy orbit integrate directly for motion.

Also attempting to train an MLP(multi-layer perceptron) to create a speedier alternative.


## Dependencies

Cuda 13.0.0 Install
```
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-ubuntu2404.pin
sudo mv cuda-ubuntu2404.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/13.0.0/local_installers/cuda-repo-ubuntu2404-13-0-local_13.0.0-580.65.06-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu2404-13-0-local_13.0.0-580.65.06-1_amd64.deb
sudo cp /var/cuda-repo-ubuntu2404-13-0-local/cuda-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update
sudo apt-get -y install cuda-toolkit-13-0
```


### Tmp Notes
Trained first orbit_mlp.pt model on training_data.
1000000 stars. 100 epochs.

~75 parsecs of error.
One shot predict, cost 0.5 sec. (Should also test batch predict.)

Far from the perfect result but still lots of things to try.
- Larger Dataset
    - Marginal improvement
        - Doesn't seem like lack of data is a major issue at the moment
- SiLU over relu
    - Significant improvement with silu
- Changing input features
    - Time --> Fourier features mapping
    - add r0 = sqrt(x0,y0)
- Edit hidden layers


Iterations
1:
- 1 Million stars * 100 timesteps, 7 features
~ 75 parsecs err
2:
- 2 Million stars * 100 timesteps, 7 features
~ 60 parsecs err

3:
- Swapped from relu to silu
~ 21 parsecs error

4:
- Added r0 and time fourier features
- Trained on 1/2 Training data (Had issues with memory)
~59 parsecs error

5:
- Trained on 3/4 of training_data + training_data_2
~ 40 parsecs error

Stopped early because it started plateauing at 40.
(also forgot to swap the learning rate when swapping data sets)
    - no longer a problem because I added load_checkpoint

6: (training_data_3, orbit_norm_6)
- training_data_3, Reduced Input Time space from -3,3 to -1,1
- Epochs, increased from 100 to 150
- scheduler changes, min_lr = 1e-6, patience=15
~16 parsecs error, after first half of data
~10 parsecs error, after second half of data
Time taken(model_test.py): 1.3s

Still room to improve here based on remaining learning rate.

6 continued:
HIDDEN=[256, 256, 256, 128] (No change from before)
- Using mixed precision brought epoch time from ~11 to ~8 seconds
~10 parsecs, after first half
    - maybe patience is too high?
~10 parsecs, after second half

- patience is too high.

7: (training_data_3, orbit_norm_6)
- Changed hidden layers:
From [256, 256, 256, 128] to [512, 512, 256, 256, 128]

- Significantly slower by ~2x, to train and to test
~10 parsecs error, after first half of data
~ 5.6 parsecs error, after second half of data
Time taken(model_test.py): 2.15

Still learning rate left to go but it's too slow.
Also Patience=15 in this one so it could be patience too high again.


8: (training_data_3, orbit_norm_6)
- Optimizer Patience reduced to 10
- HIDDEN updated to [256] * 4
- Switch to **Residual MLP**

~23 parsecs error, after first half of data

