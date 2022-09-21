import fasm


class FasmLookupError(Exception):
    '''Exception class raised upon invalid FASM entries in the FASM file.'''
    pass


class FasmInconsistentBits(Exception):
    '''Exception class raised upon any inconsistency in the produced bitstream.
    '''
    pass


class FasmAssembler(object):
    '''The template class for implementing FASM assembler.

    The class contains some basic methods for parsing FASM files, verifying
    the consistency of FASM files regarding the setting of bits, and virtual
    methods to implement for enabling features based on FASM and generating
    final bitstream.
    :param db: A Database object storing definitions for FASM features.
    :type db: Database
    :param configbits: A dictionary holding the information about set bits.
    :param configbitsline: Maps configured bit to line in FASM that set it.
    :param _configuredbit: A boolean that verifies if [set|clear]_config_bit
                           methods were used in the enable_feature function.
    '''

    def __init__(self, db):
        '''Initializes new FasmAssembler object.

        Parameters
        ----------
        db: Database
            The object holding the available features for FASM.
        '''
        self.db = db
        self.configbits = {}
        self.configbitsline = {}
        self.feature_callback = lambda feature: None
        self._configuredbit = False

    def set_feature_callback(self, feature_callback):
        '''Sets a callback function for adjusting the parsing of FASM line.

        The callback function is deployed for each FASM line and receives this
        line as an argument. It can be used for performing some additional
        actions regarding processed line.

        Parameters
        ----------
        feature_callback: A function accepting FASM line as argument.
        '''
        self.feature_callback = feature_callback

    def set_config_bit(self, coordinates, fasmline):
        '''Sets a configuration bit according to FASM line.

        It verifies if the bit on a given coordinate hasn't been cleared by
        other line. If so, it raises the FasmInconsistentBits exception.
        It stores the information about set bit in the configbits field.

        This or clear_config_bit method should be called at least once in the
        enable_feature method, otherwise FasmInconsistentBits exception will be
        raised.

        Parameters
        ----------
        coordinates:
            The coordinates of bit to set
        fasmline:
            The line that sets the bit
        '''
        assert coordinates is not None

        if coordinates in self.configbits:
            if self.configbits[coordinates] != 1:
                raise FasmInconsistentBits(
                    'FASM line "{}" wanted to set bit {} \
                     but was cleared by FASM line "{}"'
                    .format(
                        fasmline,
                        coordinates,
                        self.configbitsline[coordinates],
                    )
                )
            return
        self.configbits[coordinates] = 1
        self.configbitsline[coordinates] = fasmline
        self._configuredbit = True

    def clear_config_bit(self, coordinates, fasmline):
        '''Clears a configuration bit according to FASM line.

        It verifies if the bit on a given coordinate hasn't been set by
        other line. If so, it raises the FasmInconsistentBits exception.
        It stores the information about cleared bit in the configbits field.

        This or set_config_bit method should be called at least once in the
        enable_feature method, otherwise FasmInconsistentBits exception will be
        raised.

        Parameters
        ----------
        coordinates:
            The coordinates of bit to set
        fasmline:
            The line that sets the bit
        '''
        assert coordinates is not None

        if coordinates in self.configbits:
            if self.configbits[coordinates] != 0:
                raise FasmInconsistentBits(
                    'FASM line "{}" wanted to set bit {} \
                     but was cleared by FASM line "{}"'
                    .format(
                        fasmline,
                        coordinates,
                        self.configbitsline[coordinates],
                    )
                )
            return
        self.configbits[coordinates] = 0
        self.configbitsline[coordinates] = fasmline
        self._configuredbit = True

    def enable_feature(self, line):
        '''Enables a feature given by a FASM line.

        In this method, the feature line is processed, and according to the
        information from the Database the corresponding features are being
        set by this function.

        This function needs to be implemented by the user in the inheriting
        class.

        Parameters
        ----------
        line: FasmLine
            Parsed line from FASM file.
        '''
        raise NotImplementedError()

    def process_fasm_line(self, fasmline, missing_features):
        if not fasmline.set_feature:
            return

        self.feature_callback(fasmline.set_feature)

        try:
            self.enable_feature(fasmline)
        except FasmLookupError:
            missing_features.append(fasmline.set_feature.feature)

        if fasmline.set_feature.value != 0 and not self._configuredbit:
            raise FasmInconsistentBits(
                'The implementation of enable_feature() function did not '
                'use the set_config_bit() or clear_config_bit() '
                'for the feature: {}'.format(str(fasmline.set_feature.feature))
            )

        self._configuredbit = False

    def parse_fasm_filename(self, filename, extra_features=[]):
        missing_features = []
        for line in fasm.parse_fasm_filename(filename):
            self.process_fasm_line(line, missing_features)

        for line in extra_features:
            self.process_fasm_line(line, missing_features)

        if missing_features:
            raise FasmLookupError('\n'.join(missing_features))

    def produce_bitstream(self, outfilepath: str):
        '''Creates a file containing bitstream based on a given FASM.

        This function analyses the parsed data from a given FASM file and
        produces a bitstream that is later saved to file named outfilepath.
        This method also needs to be implemented by user, since bitstream
        format is FPGA-specific.

        Parameters
        ----------
        outfilepath: str
            The path to the output file with bitstream
        '''
        raise NotImplementedError()
