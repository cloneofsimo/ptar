# ptar: Parallel Tar Reader for Effecient Pytorch DataLoader

TBD

`ptar` is a Python package that provides a parallel tar file reader implemented using C++ and `pybind11`. This package allows efficient reading of multiple tar files containing specific file types in parallel using multiple threads that prefetch data in non-blocking fashion, just like how it is done in PyTorch's `DataLoader`. However, it is done with threads and in c++, that speeds up the process significantly.

## Features.

- **Parallel Reading**: Utilizes multiple threads to read from a list of tar files in parallel.
- **File Type Filtering**: Supports filtering by file extensions (e.g., `.txt`, `.py`, `.cpp`).
- **Python Interface**: Provides a Python interface for ease of use.

## Installation

### Prerequisites

Ensure you have the following dependencies installed:

- Python 3.9
- CMake 3.12 or higher
- `libarchive` development libraries
- `pybind11`

You can install the necessary system packages using:

```sh
sudo apt-get update
sudo apt-get install python3.9 python3.9-dev cmake libarchive-dev
pip install pybind11 setuptools
```

### Virtual Environment Setup

1. **Create and activate a virtual environment**:

```sh
python3 -m venv myenv
source myenv/bin/activate
```

2. **Ensure Python development headers are accessible**:

```sh
mkdir -p myenv/include/python3.9
ln -s /usr/include/python3.9/* myenv/include/python3.9/
mkdir -p myenv/lib
ln -s /usr/lib/x86_64-linux-gnu/libpython3.9.so myenv/lib/
```

### Building and Installing `ptar`

1. **Clone the repository**:

```sh
git clone https://github.com/cloneofsimo/ptar.git
cd ptar
```

2. **Install the package**:

```sh
python setup.py install
```

