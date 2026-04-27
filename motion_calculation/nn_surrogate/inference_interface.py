from orbit_mlp import (
    load_model_from_file,
    load_norm_stats,
    predict_batch
)

class InferenceInterface():
    """
        Interface to abstract orbit mlp loading and prediction.
    """

    def __init__(self, config):
        self.model = load_model_from_file(config)
        self.norm_stats = load_norm_stats(config)
        self.stars = None

    def get_positions_at_t(self, stars: list, t: float):
        """
        """

        return predict_batch(self.mode, self.norm_stats, stars)
    
