# Google APIs

This creates a conan package which contains the protos for api & rpc subfolders.

The Justfile clones and does a sparse-checkout of just those two folders. It obtains
the date and revision of the last commit and uses that to tag the conan reference.

To update the Conan reference to a new release:
```bash
just build
```

To re-export the Conan reference from the *previously* cloned googleapis sub-folder
```bash
./export.sh
```