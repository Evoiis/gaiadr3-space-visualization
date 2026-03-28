import zmq
import star_data_pb2

# sd = star_data_pb2.StarData()
# print(sd.SerializeToString())
# k = star_data_pb2.StarData()
# k.ParseFromString(sd.SerializeToString())


class DownloadNode():

    def __init__(self, download_wrapper, data_procesor, port: int):
        """
        Input:
        - download_wrapper, class to get data from
        - port, port number
        """
        self.zmq_context = zmq.Context()
        self.socket = zmq_context.socket(zmq.REP)
        self.socket.bind(f"tcp://*:{port}")

        self.download_wrapper = download_wrapper

    def loop(self):

        try:
            while True:

                # Wait for msg
                message = self.socket.recv()

                print("Received message: ", message)

                self.socket.send("Reply")

        except KeyboardInterrupt:
            pass

