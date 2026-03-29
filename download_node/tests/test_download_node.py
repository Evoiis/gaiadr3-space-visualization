import pytest
import zmq
import threading
import time
import logging

import star_data_pb2

from node import DownloadNode

class MockDownloadWrapper():

    def __init__(self, string):
        self.string = string

    def get_data(self):
        return self.string

class MockDataProcessor():
    def process_data(self, data):
        stars = star_data_pb2.Stars()
        stars.timestamp = int(time.time())

        entry = stars.stars[1001]
        entry.pos_x = 1.5
        entry.pos_y = -0.3
        entry.pos_z = 2.1
        entry.brightness = 0.8
        entry.size = 3.0
        entry.color_r = 120
        entry.color_g = 200
        entry.color_b = 255

        entry = stars.stars[1002]
        entry.pos_x = -0.5
        entry.pos_y = 0.7
        entry.pos_z = -1.2
        entry.brightness = 0.4
        entry.size = 5.0
        entry.color_r = 255
        entry.color_g = 100
        entry.color_b = 50

        return stars.SerializeToString()

TEST_SERVER_PORT = 5556
TEST_STRING = "TEST STRING"
logger = logging.getLogger(__name__)

def test_download_node_req():
    mdw = MockDownloadWrapper(TEST_STRING)
    mdp = MockDataProcessor()
    dnode = DownloadNode(mdw, mdp, TEST_SERVER_PORT)
    dnode_thread = threading.Thread(target=dnode.run_node)
    dnode_thread.start()

    zmq_context = zmq.Context()
    socket = zmq_context.socket(zmq.REQ)
    socket.setsockopt(zmq.RCVTIMEO, 5000)
    logger.debug("Init Complete, now connect")
    socket.connect(f"tcp://localhost:{TEST_SERVER_PORT}")

    data_req = star_data_pb2.DataRequest()
    data_req.timestamp = int(time.time())
    data_req.node_name = "Test Node"
    socket.send(data_req.SerializeToString())
    logger.debug("Sent waiting for reply.")

    try:
        message = socket.recv()
    except zmq.Again:
        dnode.stop_loop()
        dnode_thread.join()
        pytest.fail("Client recv timed out while waiting for a reply from server.")

    received_stars = star_data_pb2.Stars()
    received_stars.ParseFromString(message)
    logger.debug(f"Received {len(received_stars.stars)} stars")

    dnode.stop_loop()
    dnode_thread.join()

    assert received_stars.timestamp > 0
    assert len(received_stars.stars) == 2

    s1 = received_stars.stars[1001]
    assert s1.pos_x == pytest.approx(1.5, abs=1e-6)
    assert s1.pos_y == pytest.approx(-0.3, abs=1e-6)
    assert s1.pos_z == pytest.approx(2.1, abs=1e-6)
    assert s1.brightness == pytest.approx(0.8, abs=1e-5)
    assert s1.size == pytest.approx(3.0, abs=1e-5)
    assert s1.color_r == 120
    assert s1.color_g == 200
    assert s1.color_b == 255

    s2 = received_stars.stars[1002]
    assert s2.pos_x == pytest.approx(-0.5, abs=1e-6)
    assert s2.pos_y == pytest.approx(0.7, abs=1e-6)
    assert s2.pos_z == pytest.approx(-1.2, abs=1e-6)
    assert s2.brightness == pytest.approx(0.4, abs=1e-5)
    assert s2.size == pytest.approx(5.0, abs=1e-5)
    assert s2.color_r == 255
    assert s2.color_g == 100
    assert s2.color_b == 50
