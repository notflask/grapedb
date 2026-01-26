<div style="display: flex; align-items: end;">
    <img src="./assets/grapedb.png" style="max-width: 64px; margin-right: 8px;" />
    <h1 style="margin: 0; border: none;">GrapeDB</h1>
</div>

### A lightweight, persistent key-value database written in modern C++

## About

GrapeDB is a simple yet efficient embedded key-value storage engine designed for learning and understanding database internals. It implements core database concepts including:

- Persistent storage with append-only log structure
- Automatic compaction to reclaim disk space
- Crash recovery to ensure data integrity
- Thread-safe operations using mutex locks
- Memory indexing for fast key lookups

Perfect for educational purposes, small projects, or as a foundation for understanding how databases work under the hood.

## Features

- Simple API - Easy-to-use `Set`, `Get`, `Delete` operations
- Persistence - Data survives application restarts
- Auto-compaction - Configurable threshold for garbage collection
- Crash-safe - Atomic operations with recovery mechanism
- Thread-safe - Concurrent access protected by mutexes
- CLI Tool - Interactive command-line interface included
- Comprehensive tests - Full test coverage with Google Test
- Modern C++ - Uses C++17/23 features

## Quick Start
```cpp
#include "grapedb/database.h"

int main() {
    Grape::Database db("my_database.grape");
    
    // Store data
    db.Set("username", "alice");
    db.Set("email", "alice@example.com");
    
    // Retrieve data
    std::string username = db.Get("username");  // "alice"
    
    // Delete data
    db.Delete("email");
    
    // Manual compaction
    db.Compact();
    
    return 0;
}
```

## Installation

### Prerequisites

- CMake 3.10 or higher
- GCC 14+ or Clang 16+ (for C++17/23 support)
- Git

### Build from Source
```bash
# Clone the repository
git clone https://github.com/yourusername/grapedb.git
cd grapedb

# Create build directory
mkdir build && cd build

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Run tests
./grapedb_tests

# Run CLI
./grapecli/grapecli --path mydb.grape
```

## Usage

### Using the Library

#### Basic Operations
```cpp
#include "grapedb/database.h"

Grape::Database db("data.grape");

// Write operations
db.Set("key1", "value1");
db.Set("key2", "value2");

// Read operation
std::string value = db.Get("key1");
if (value.empty()) {
    std::cout << "Key not found" << std::endl;
}

// Delete operation
if (db.Delete("key1")) {
    std::cout << "Deleted successfully" << std::endl;
}
```

#### List All Keys
```cpp
auto keys = db.ListKeys();
for (const auto& key : keys) {
    std::cout << key << " = " << db.Get(key) << std::endl;
}
```

#### Configure Auto-Compaction
```cpp
// Set compaction threshold to 5MB
db.SetCompactionThreshold(5 * 1024 * 1024);

// Manual compaction
db.Compact();
```

### Using the CLI Tool
```bash
# Start interactive shell
./grapecli --path mydb.grape --threshold 10485760

grape> SET username john_doe
grape> SET email john@example.com
grape> GET username
'username' = 'john_doe'
grape> LIST
found 2 key(s):
- username
- email
grape> DELETE email
grape> COMPACT
grape> EXIT
```

#### CLI Commands

| Command | Description | Example |
|---------|-------------|---------|
| `SET key value` | Store a key-value pair | `SET name Alice` |
| `GET key` | Retrieve value by key | `GET name` |
| `DELETE key` | Remove a key | `DELETE name` |
| `LIST` | Show all keys | `LIST` |
| `COMPACT` | Manually compact database | `COMPACT` |
| `THRESHOLD size` | Set compaction threshold | `THRESHOLD 5242880` |
| `EXIT` | Quit the CLI | `EXIT` |

## Testing
```bash
# Run all tests
./grapedb_tests

# Run with verbose output
./grapedb_tests --gtest_filter=DatabaseTest.*
```

### Test Coverage

- Serialization/deserialization
- Basic CRUD operations
- Persistence across restarts
- Compaction functionality
- Auto-compaction triggering
- Crash recovery scenarios
- Concurrent access (thread safety)
- Edge cases (empty values, corrupted data)

## Performance

Benchmark on a typical development machine:
```bash
./grapedb_benchmark
```

Example output:
```
Writes per second: ~85,000 ops/sec
```

Note: Performance varies based on disk speed, record size, and system load.

## Project Structure
```
grapedb/
├── grapedb/              # Core database library
│   ├── database.h/cpp    # Main database class
│   ├── serializer.h/cpp  # Record serialization
│   ├── record.h          # Record structure
│   └── logger.h/cpp      # Logging utilities
├── grapecli/             # CLI application
│   └── main.cpp          # Interactive shell
├── tests/                # Unit tests
│   ├── test_database.cpp
│   └── test_serializer.cpp
├── benchmark.cpp         # Performance benchmark
├── CMakeLists.txt        # Build configuration
└── README.md
```

## Configuration

### CMake Options
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Specify compiler
cmake -DCMAKE_CXX_COMPILER=g++-14 ..
```

### Logging

GrapeDB uses spdlog for logging. Logs are written to:
- Console: Colored output with timestamps
- File: `grapedb.log` (persistent)

## Contributing

Contributions are welcome! Here's how you can help:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow the existing code style (see `.clang-format`)
- Add tests for new features
- Update documentation as needed
- Ensure all tests pass before submitting

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

Danilo Skyba
- GitHub: [@notflask](https://github.com/notflask)
- Email: skyba@gmx.net

## Acknowledgments

- Inspired by LevelDB and RocksDB
- Built with spdlog for logging
- Uses GoogleTest for testing
- CLI powered by cxxopts