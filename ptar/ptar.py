import cptar

class Ptar:
    def __init__(self, shardlist, schema):
        self.reader = cptar.ParallelTarReader(shardlist, 10 * len(shardlist), len(shardlist), schema)
        self.schema = schema

    def get_next(self):
        data = self.reader.get_next_n_data(1)
        buffer = data[0]
        return buffer

    def get_next_n(self, n):
        return self.reader.get_next_n_data(n)

    def reset(self):
        self.reader.reset()
