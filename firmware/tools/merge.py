#!/usr/bin/env python3

import argparse
from intelhex import IntelHex
import sys
import os.path

def infer_type(file_name):
    (root, ext) = os.path.splitext(file_name)
    if ext in ['.bin', '.hex']:
        return ext[1:]
    is_hex = all(map(lambda x: chr(x) in string.hexdigits + ':\r\n', data))
    return 'hex' if is_hex else 'bin'

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-o' '--output-file', dest='output_file', default='-',
        help='Output hex file (default stdout)')
    parser.add_argument('--fill', dest='fill_byte', default='0xFF')
    parser.add_argument('--start-address', default=None,
        help='Ignore input contents before this address')
    parser.add_argument('input_files', nargs='+',
        help='Input hex or bin files (bin files must be followed by offset)')
    args = parser.parse_args()

    if args.output_file == '-':
        args.output_file = sys.stdout

    fill_byte = int(args.fill_byte, 0)

    merged = IntelHex()
    merged.padding = fill_byte
    i = 0
    while i < len(args.input_files):
        file = args.input_files[i]
        if infer_type(file) == 'hex':
            merged.merge(IntelHex(file), overlap='replace')
            i += 1
        else:
            offset = int(args.input_files[i+1], 0)
            new = IntelHex()
            new.loadbin(file, offset=offset)
            merged.merge(new, overlap='replace')
            i += 2

    if args.start_address:
        start_address = int(args.start_address, 0)
        del merged[:start_address]

    try:
        if args.output_file.endswith('.bin'):
            merged.tobinfile(args.output_file)
        else:
            merged.write_hex_file(args.output_file, False)
    except BrokenPipeError:
        pass

if __name__ == '__main__':
    main()
