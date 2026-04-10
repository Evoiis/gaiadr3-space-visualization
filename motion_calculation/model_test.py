from orbit_mlp import load_model_from_file, OrbitDataset, evaluate_with_full_dataset_on_gpu, loss_to_parsecs, load_config, load_norm_stats, flogger
import torch
import torch.nn as nn

import time
import os

DEVICE = "cuda" if torch.cuda.is_available() else "cpu"
CONFIG_FILE = "config_14.yaml"

print("\nLoading model for test evaluation...")

os.environ["PYTORCH_CUDA_ALLOC_CONF"] = "expandable_segments:True"
torch.set_float32_matmul_precision("high")

flogger.set_write_to_file(False)
config = load_config(CONFIG_FILE)


TEST_DATA_PATH = "test_data_3"
NORM_PATH = config["norm_path"]
MODEL_PATH = config["model_name"]

if not os.path.exists(MODEL_PATH):
    MODEL_PATH = "./prev_models/" + config["model_name"]

# OVERRIDE
NORM_PATH = "orbit_norm_6.json"
MODEL_PATH = "./prev_models/orbit_mlp_7.pt"

print(f"\nRunning model test on: {MODEL_PATH}")
print(f"Using norms: {NORM_PATH}\n")
model = load_model_from_file(MODEL_PATH)
norm_stats = load_norm_stats(NORM_PATH)

test_set = OrbitDataset(TEST_DATA_PATH, norm_stats)
test_X = test_set.X.to(DEVICE)
test_y = test_set.y.to(DEVICE)

loss_fn = nn.MSELoss()

# warmup
# with torch.no_grad():
#     _ = model(test_X[:config["batch_size"]])


start = time.time()
test_loss = evaluate_with_full_dataset_on_gpu(model, test_X, test_y, loss_fn, batch_size=config["batch_size"])

print(f"Time taken: {time.time() - start}")
print(f"{len(test_X)=}")

test_pc = loss_to_parsecs(test_loss, norm_stats)

print(f"{test_pc=} parsecs")
