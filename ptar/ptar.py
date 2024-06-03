import cptar

class Ptar:
    def __init__(self, shardlist, schema):
        self.reader = cptar.ParallelTarReader(shardlist, 10 * len(shardlist), len(shardlist))
        self.schema = schema

    def get_next(self):
        data = self.reader.get_next_n_data(1)
        if not data:
            return None, None
        
        buffer, filename = data[0]
        for ext in self.schema:
            if filename.endswith(ext):
                return buffer, filename
        return None, None
