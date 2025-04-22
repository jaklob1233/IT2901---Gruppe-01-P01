import os
from pathlib import Path
from funasr import AutoModel
from funasr.utils.postprocess_utils import rich_transcription_postprocess
from huggingface_hub import snapshot_download

script_dir = Path(__file__).resolve().parent
superb_dir = script_dir / "Superb"

# This will download the entire repository to the specified directory
#snapshot_download(repo_id="superb/wav2vec2-large-superb-er", local_dir=str(superb_dir), repo_type="model")

# Set the cache directory where the model will be downloaded and stored
os.environ["MODELSCOPE_CACHE"] = os.path.join(os.path.dirname(__file__), "Sensevoice", "model")

# Define model to download
model_name = "iic/SenseVoiceSmall"

# Download and save the model
model = AutoModel(
    model=model_name,
    vad_model="iic/speech_fsmn_vad_zh-cn-16k-common-pytorch",
    vad_kwargs={"max_single_segment_time": 30000},
    trust_remote_code=True
)

