import os
import tarfile
from ptar import Ptar

def create_sample_files(directory, index):
    os.makedirs(directory, exist_ok=True)
    filenames = [f"a_{index}.txt", f"b_{index}.py", f"c_{index}.cpp"]
    contents = ["This is file a", "print('This is file b')", "int main() { return 0; }"]

    for filename, content in zip(filenames, contents):
        with open(os.path.join(directory, filename), 'w') as f:
            f.write(content)

    return filenames

def create_tar_file(tar_name, directory, filenames):
    with tarfile.open(tar_name, "w:gz") as tar:
        for filename in filenames:
            tar.add(os.path.join(directory, filename), arcname=filename)

def create_sample_tar_files():
    for i in range(5):
        dir_name = f"sample_dir_{i}"
        tar_name = f"shard{i}.tar.gz"
        filenames = create_sample_files(dir_name, i)
        create_tar_file(tar_name, dir_name, filenames)
        # Clean up the sample directories
        for filename in filenames:
            os.remove(os.path.join(dir_name, filename))
        os.rmdir(dir_name)

def test_ptar():
    shardlist = [f"shard{i}.tar.gz" for i in range(5)]
    schema = ['.txt', '.py', '.cpp']

    reader = Ptar(shardlist, schema)

    all_data = []
    while True:
        data, filename = reader.get_next()
        if data is None:
            break
        print(f"Read file: {filename} with size: {len(data)} bytes")
        all_data.append((data, filename))

    # Verify content
    for i in range(5):
        for ext in ['a', 'b', 'c']:
            filename = f"{ext}_{i}.txt" if ext == 'a' else f"{ext}_{i}.py" if ext == 'b' else f"{ext}_{i}.cpp"
            expected_content = "This is file a" if ext == 'a' else "print('This is file b')" if ext == 'b' else "int main() { return 0; }"
            matching_files = [file for content, file in all_data if file == filename]
            if matching_files:
                print(f"Found {filename} with expected content.")
            else:
                print(f"Missing {filename}.")

def clean_up_tar_files():
    for i in range(5):
        tar_name = f"shard{i}.tar.gz"
        if os.path.exists(tar_name):
            os.remove(tar_name)

if __name__ == "__main__":
    create_sample_tar_files()
    test_ptar()
    clean_up_tar_files()
