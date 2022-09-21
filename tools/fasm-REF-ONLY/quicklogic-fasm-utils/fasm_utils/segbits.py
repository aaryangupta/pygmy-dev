from collections import namedtuple


Bit = namedtuple('Bit', 'x y isset')


def parsebit(val: str):
    """Parses bit notation for .db files to Bit class.

    Parameters
    ----------
    val: str
        A string containing .db bit notation, i.e. "!012_23" => (12, 23, False)

    Returns
    -------
    Bit: A named tuple Bit with parsed word column, word bit and value

    """
    isset = True
    # Default is 0. Skip explicit call outs
    if val[0] == '!':
        isset = False
        val = val[1:]
    # 28_05 => 28, 05
    seg_word_column, word_bit_n = val.split('_')

    return Bit(
        x=int(seg_word_column),
        y=int(word_bit_n),
        isset=isset,
    )


def read_segbits_line(line: str):
    '''Parses segbits from line.'''
    linestrip = line.strip()
    if linestrip:
        parts = linestrip.split(' ')
        assert len(parts) > 1
        return parts[0], [parsebit(val) for val in parts[1:]]


def read_segbits_file(filepath: str):
    """Parses bits from the lines of the .db file.

    Parameters
    ----------
    f: str
        A path to .db file.

    Returns
    -------
    dict of str: Bit: Dictionary containing parsed .db file.
    """
    segbits = {}

    with open(filepath, 'r') as f:
        for l in f:
            # CLBLM_L.SLICEL_X1.ALUT.INIT[10] 29_14
            name, bits = read_segbits_line(l)
            segbits[name] = bits

    return segbits
