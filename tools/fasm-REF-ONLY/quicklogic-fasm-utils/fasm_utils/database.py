from .db_entry import DbEntry
import itertools


class Table(object):
    '''Represents single DB file in the database.
    '''
    def __init__(self, filepath: str):
        '''Creates Table from a DB file.

        Parameters
        ----------
        filepath: str
            Path to the DB file.
        '''
        self.filepath = filepath
        self.mapping = {}
        with open(filepath, 'r') as f:
            for l in f:
                entry = DbEntry.from_dbline(l)
                self.mapping[entry.signature] = entry

    def get_entry(self, featurename: str):
        '''Returns a matching DB entry for feature name or None.

        Parameters
        ----------
        featurename: str
            Name of the feature from FASM file.

        Returns
        -------
        DbEntry: corresponding DbEntry or None if there is no such DbEntry
        '''
        if featurename not in self.mapping:
            return None
        return self.mapping[featurename]

    def __iter__(self):
        return self.mapping.values().__iter__()


class Database(object):
    '''Contains all parsed DB files required to generate bitstream from FASM.
    '''
    def __init__(self, db_root: str):
        self.db_root = db_root
        self.tables = {}

    def add_table(self, tablename: str, filepath: str):
        self.tables[tablename] = Table(filepath)

    def get_table(self, tablename: str):
        return self.tables[tablename]

    def get_feature_from_table(self, tablename: str, featurename: str):
        return self.tables[tablename].get_entry(featurename)

    def get_feature(self, featurename: str):
        for table in self.tables.values():
            feature = table.get_entry(featurename)
            if feature is not None:
                return feature
        return None

    def __iter__(self):
        return itertools.chain.from_iterable(self.tables.values())
