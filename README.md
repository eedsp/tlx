# tlx

Library to tokenize text (UTF-8) using regular expression.

## Requirements
* [CMake][cmake] -- Build, test and package software
* [PCRE2][pcre2] -- Perl Compatible Regular Expressions
* [spdlog][spdlog] -- Super fast C++ logging library
* [ICU][icu] -- Library for Unicode and Globalization
* [Jansson][jansson] -- C library for working with JSON 
* [APR][apr] -- Apache Portable Runtime
* [APR-util][apr-util] -- Apache Portable Runtime Utility

[pcre2]: http://pcre.org/
[spdlog]: https://github.com/gabime/spdlog
[cmake]: https://cmake.org
[icu]: http://site.icu-project.org/
[jansson]: http://www.digip.org/jansson/
[apr]: http://apr.apache.org/
[apr-util]: http://apr.apache.org/

## Build
```shell
mkdir ./build
cd ./build
cmake ..
make [-j]
```
## Testing
### Text tokenizer
#### Create a dictionary for text segmentation 
```shell
cd ./scripts
./build.dic.sh

# check dictionary files
ls ../db
   phrase.db       token.sgmt.db
```

#### Import dictionary files into shared memory
```shell
cd ./scripts
./db.import.sh
```

#### Run test
```shell
cd ./tests

# test script for text tokenizer and extract phrase pattern from text
./test.phrase.sh

# test script for text tokenizer
./test.token.sh
```

#### Remove dictionary from shared memory
```shell
cd ./scripts
./db.free.sh
```

## License
Licensed under an [Apache-2.0](https://github.com/dmlc/mxnet/blob/master/LICENSE) license.
