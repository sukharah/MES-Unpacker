## Purpose
This program allows lossless conversion between MES dialog bytecode archives and a plaintext format
for the game cube version of Harvest Moon: A Wonderful Life and
Harvest Moon: Another Wonderful Life

The text format is not intended to be compatible with other text formats, or other versions of this program,
but should allow the corresponding version to generate a valid MES file so long as the editor is limited
to the bytecode vocabulary supported by the MES file.

Currently the only text encoding supported by this version is UTF-8.

This program was inspired by [Harrison's MES editor](https://github.com/harrison-h/.mes-Editor).

## To compile
Install either g++ (packaged with MinGW for Windows) or some other c++ compiler to compile this program.

The minimal command line for g++ for this program:
```
g++ -std=c++11 -iquote header "source/main.cpp" "source/MES.cpp"
```

To allow the program to perform sequential deduplication of generated MES files,
add the `-DDUPE` compiler flag.

## To run
The program takes 3 parameters and one optional parameter
It exposes the following two functions:

To extract the contents of an MES file as text
```
program.exe unpack "./mesinputfile.mes" "./textoutputfile.txt" ["data/bytecodedef.txt"]
```

To generate an MES file from the text file
```
program.exe pack "./textinputfile.txt" "./mesoutputfile.mes" ["data/bytecodedef.txt"]
```

The `data/bytecodedef.txt` file specifies which bytecode/character set definitions should be used for
processing the files. For example, for Harvest Moon: A Wonderful Life files, the parameter should be
`data/awl.txt`, which is the default value. For Harvest Moon: Another Wonderful Life it should be
specified as `data/anawl.txt`.

## Additional development/Notes
The following files for Harvest Moon: Another Wonderful Life are corrupted and will not be supported:
- All *_wife.mes files.
- debug.mes
- phrase.mes
The files use a sequential mapping in order encountered in file to the Japanese characters from AWL.
Characters beyond the range of the single byte text symbol mapping used in Another Wonderful Life are
arbitrarily mapped to 2 bytes, which conflicts with lower message codes, making the encoding irreversible
except through manual decoding. Aside from some haphazard English translations (and 'error') in debug.mes,
there are no other changes in these files.

Planned features/fixes:
- Add error messages.
- Verify behavior with source, add any bytecodes not in test set.