from ptar import Ptar

shardlist = [f"shard{i}.tar.gz" for i in range(5)]
# tar has 3 files with extensions .txt, .py, .cpp, in order.

schema = ['.txt', '.py', '.cpp']

reader = Ptar(shardlist, schema)

all_data = []
data = reader.get_next_n(5)
print(data)
for i in range(5):
    data = reader.get_next()
    print(data)
    # this will return all 3 of them.

# Make the reader iterate from beginning.
reader.reset()
data = reader.get_next_n(5)
# return all 5 of them all at once.
print(data)
