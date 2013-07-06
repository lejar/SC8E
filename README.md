To build the project:

```
mkdir build
cd build
cmake ..
make
```

To build and run the unit tests:

```
make tests
```

When running `cmake` with an additional `-Dtest=ON` parameter, the tests are
built and run automatically whenever `make` is run.

Enjoy!
