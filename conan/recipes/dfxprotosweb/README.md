# DFX Proto APIs

This creates a conan package which contains the protos for DFX API

The Justfile clones from private repositories and extracts just the
public proto information.

To update the Conan reference to a new release:
```bash
just build
```

To re-export the Conan reference from the *previously* cloned include/protos folders
```bash
./export.sh
```