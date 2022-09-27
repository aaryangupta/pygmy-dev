from . import segbits


class DbEntry(object):
    '''Class representing single entry in FASM database.
    '''

    def __init__(self, signature: str, coords: list):
        self.signature = signature
        self.coords = coords

    @classmethod
    def from_dbline(cls, dbline: str) -> 'DbEntry':
        '''Creates DbEntry from single line of the DB file.

        Parameters
        ----------
        dbline: str
            A line from DB file.
        '''
        signature, coords = segbits.read_segbits_line(dbline)
        return cls(signature, coords)

    def __str__(self):
        res = self.signature
        for coord in self.coords:
            res += ' {}{}_{}'.format(
                    '' if coord.isset else '!',
                    coord.x, coord.y)
        res += '\n'
        return res
