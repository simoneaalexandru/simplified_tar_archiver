# Simplified TAR Archiver (MyTar)

A simplified implementation of the `tar` utility in C, compatible with GNU tar archives (USTAR).
The project focuses on low-level binary file handling using `fread`/`fwrite`, manual construction of TAR headers,
checksum computation, and 512-byte block alignment (no compression).

## Features
- Create TAR archives
- Extract TAR archives
- USTAR header format (512-byte blocks)
- Correct header checksum calculation
- Proper padding to 512-byte boundaries
- Compatible with GNU tar

## Usage

### Create an archive
```bash
./create_archive myarchive.tar file1 file2 file3 ...

###Extract an archive
```bash
./extrac_archive myarchive.tar