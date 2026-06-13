# Internal Design

## Core Philosophy

Configuration loading and configuration usage
must be completely separated.

Initialization Phase:

    File
      ↓
    Parse
      ↓
    Validate
      ↓
    Freeze

Runtime Phase:

    Read Only

No runtime parsing is allowed.

---

# Storage Model

V1 storage model:

unordered_map<string, ConfigValue>

Lookup:

O(1)

Memory:

Owned entirely by ConfigSystem.

---

# Configuration Lifecycle

Load()

    Parse file
    Validate schema
    Build storage
    Freeze snapshot

Get<T>()

    Read immutable storage

Supported V1 Value Types:

- bool
- int64
- double
- string

All other types must be user-composed from these primitives.

Get<T>() returns const reference or value.

Mutable access is forbidden.

---

# Snapshot Model

All configuration data is immutable after loading.

No mutable references are exposed.

---

# Parsing Strategy

V1:

JSON only

Future:

JSON
YAML
TOML
Binary Config

---

# Failure Philosophy

Configuration errors are startup errors.

Configuration errors are not recoverable.

Fail fast.
