import logging

from src.gaia_data_processor import GaiaDataProcessor
from src.gaia_query import GaiaQueryParameters, GaiaQueryWrapper
from src.node import DownloadNode

def main():
    logging.basicConfig(level=logging.INFO)

    gqw = GaiaQueryWrapper(GaiaQueryParameters(
        n_stars_per_batch=250000,
        guarantee_rad_velocity=True,
        # random_set_modulo=50,
        # phot_g_mean_mag_upper_bound=8
    ))

    gdp = GaiaDataProcessor("data/") # TODO data folder path launch param

    dnode = DownloadNode(
        gqw,
        gdp,
        5656,
        preload_data=True,
        n_batches=5
    ) # TODO launch params

    dnode.run_node()


if __name__ == "__main__":
    main()
