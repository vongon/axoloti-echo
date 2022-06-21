#!/usr/bin/env python3

import argparse
from intelhex import IntelHex
import struct
import zlib
import sys

class MissingHeaderError(Exception): pass

class PackCRC:

    def __init__(self, ihex, offset=0, size=0, seed=0, fill=0xFF):
        self._ihex = ihex
        self._ihex.padding = fill

        if size:
            self._pad_to_size(offset, size, fill)
        else:
            size = self._ihex.maxaddr() + 1 - offset

        header = self._find_header(offset)
        if header is None:
            raise MissingHeaderError

        # Write the image length word
        self._write_word(header + 8, size)

        # Skip the checksum word when calculating the checksum
        section1 = ihex[offset:(header + 12)].tobinstr()
        section2 = ihex[(header + 16):(offset + size)].tobinstr()
        checksum = zlib.crc32(section1 + section2, seed) & 0xFFFFFFFF

        # Write the image checksum word
        self._write_word(header + 12, checksum)

    def _write_word(self, addr, word):
        self._ihex[addr:(addr + 4)] = list(struct.pack('<L', word))

    def _pad_to_size(self, offset, size, fill):
        enda = self._ihex.maxaddr() + 1
        endb = offset + size
        if endb > enda:
            self._ihex[enda:endb] = [fill] * (endb - enda)

    def _find_header(self, offset):
        # The image header should immediately follow the vector table which
        # is placed at the beginning of the image.
        # 1 word stack pointer + max 256 vectors + 2 words marking the header
        num_words = 1 + 256 + 2
        words = struct.unpack_from('<{}L'.format(num_words),
            self._ihex[offset:].tobinstr())

        for (i, a, b) in zip(range(len(words)), words, words[1:]):
            if (a, b) == (0xFF01FF02, 0xFF03FF04):
                return offset + (4 * i)

    def packed(self):
        return self._ihex

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-x', dest='offset', default='0',
        help='Ignore contents before this location (default 0)')
    parser.add_argument('-b', dest='base', default='0',
        help='Offset is relative to this address (default 0)')
    parser.add_argument('-e', dest='crc_seed', default='0',
        help='Image CRC32 seed (default 0)')
    parser.add_argument('-f', dest='fill_byte', default='0xFF',
        help='Fill/pad byte (default 0xFF)')
    parser.add_argument('-s', dest='image_size', default='0',
        help='If given, truncate or pad the image to this size')
    parser.add_argument('-o', dest='output_file', default='-',
        help='Output hex file (default stdout)')
    parser.add_argument('-i', dest='input_file', default='-',
        help='Input hex file (default stdin)')
    args = parser.parse_args()

    if args.output_file == '-':
        args.output_file = sys.stdout

    if args.input_file == '-':
        args.input_file = sys.stdin

    ihex = PackCRC(IntelHex(args.input_file),
        offset=int(args.base, 0)+int(args.offset, 0),
        size=int(args.image_size, 0),
        seed=int(args.crc_seed, 0),
        fill=int(args.fill_byte, 0)).packed()

    try:
        ihex.write_hex_file(args.output_file, False)
    except BrokenPipeError:
        pass

if __name__ == '__main__':
    main()
