## Purpose
This program allows lossless conversion between MES dialog bytecode archives and a plaintext format.
The text format is not intended to be compatible with other text formats, or other versions of this program,
but should allow the corresponding version to generate a valid MES file so long as the editor is limited
to the bytecode vocabulary supported by the MES file.

Currently the only text encoding supported by this version is UTF-8.

## To compile
Install either g++ (packaged with MinGW for Windows) or some other c++ compiler to compile this program.

The minimal command line for g++ for this program:
```
g++ -std=c++11 -iquote header "source/main.cpp" "source/MES.cpp"
```

To allow the program to perform sequential deduplication of generated MES files,
add the `-DDUPE` compiler flag.

## To run
The program takes 3 parameters, and exposes the following two functions

To extract the contents of an MES file as text
```
program.exe unpack "./mesinputfile.mes" "./textoutputfile.txt"
```

To generate an MES file from the text file
```
program.exe pack "./textinputfile.txt" "./mesoutputfile.mes"
```

If structure.mes/people.mes files are located in the program directory,
generated text files will use names extracted from those files for the STRUCTURE/PEOPLE
bytecodes.

## Additional development

Planned features/fixes:
- Add support for other parameter formats for bytecode entities.
- Add names for 'unknown' bytecode instructions.
- Add error messages.
- Add additional printable characters from font table.
- Verify behavior with source, add any bytecodes not in test set.