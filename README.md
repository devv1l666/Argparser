# ArgParser

**ArgParser** is a lightweight C++11+ command-line argument parser that provides a flexible and user-friendly way to define and parse command-line arguments in CLI applications.

---

## Features

- Support for:
  - **Flags** (`--verbose`, `-v`)
  - **Integer arguments** (`--count 10`, `--count=10`)
  - **String arguments** (`--name John`, `--name=John`)
  - **Positional arguments**
  - **Multi-value arguments**
  - **Help option** (`--help`, `-h`)
- `Default` values for any argument type
- Automatic storage of parsed values
- Friendly `--help` text generator
- Short (`-x`) and long (`--example`) option formats
- Support for glued short flags like `-abc`

---

## Getting Started

### 1. Include the parser:

```cpp
#include "ArgParser.h"
```

### 2. Create parser and define arguments:

```cpp
ArgumentParser::ArgParser parser("MyApp");

int count = 0;
parser.AddIntArgument('c', "count", "Number of repetitions").StoreValue(count).Default(1);

bool verbose = false;
parser.AddFlag('v', "verbose", "Enable verbose output").StoreValue(verbose);

std::string name;
parser.AddStringArgument("name", "Name of the user").StoreValue(name);

parser.AddHelp('h', "help", "Show help message");
```

### 3. Parse command-line arguments:

```cpp
if (!parser.Parse(argc, argv)) {
    std::cerr << "Invalid arguments. Use --help for usage.\n";
    return 1;
}

if (parser.Help()) {
    std::cout << parser.HelpDescription();
    return 0;
}
```

---

## Example

```bash
./MyApp --count 5 --name=John -v
```

Parsed values:
- `count = 5`
- `name = "John"`
- `verbose = true`

---

## Build
No external dependencies. Just include the `.h` and `.cpp` files in your project.

---

## License
MIT License
