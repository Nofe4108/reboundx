#FOLLOWING SHOWS WHAT I THINK IS NECESSARY FOR OUR C VERSION

import os
from os.path import join
import re

import numpy as np


class MesaData:


    header_names_line = 2
    bulk_names_line = 6

    @classmethod
    def set_header_name_line(cls, name_line=2):
        cls.header_names_line = name_line

    @classmethod
    def set_data_rows(cls, name_line=6):
        cls.bulk_names_line = name_line

    def __init__(self, file_name=join('.', 'LOGS', 'history.data'),
                 file_type=None):
        """Make a MesaData object from a Mesa output file.

        Reads a profile or history output file from mesa. Assumes a file with
        the following structure:

        line 1: header names
        line 2: header data
        line 3: blank
        line 4: main data names
        line 5: main data values

        This structure can be altered by using the class methods
        `MesaData.set_header_rows` and `MesaData.set_data_rows`.

        Parameters
        ----------
        file_name : str, optional
            File name to be read in. Default is 'LOGS/history.data'

        file_type : str, optional
            File type of file to be read.  Default is None, which will
            auto-detect the type based on file extension.  Valid values are
            'model' (a saved model) and 'log' (history or profile output).
        """
        self.file_name = file_name
        self.file_type = file_type
        self.bulk_data = None
        self.bulk_names = None
        self.header_data = None
        self.header_names = None
        self.read_data()

    
    def read_log_data(self):
        """Reads in or update data from the original log (.data or .log) file.

        This re-reads the data from the originally-provided file name. Mostly
        useful if the data file has been changed since it was first read in or
        if the class methods MesaData.set_header_rows or MesaData.set_data_rows
        have been used to alter how the data have been read in.

        Returns
        -------
        None
        """
        self.bulk_data = np.genfromtxt(
            self.file_name, skip_header=MesaData.bulk_names_line - 1,
            names=True, dtype=None)
        self.bulk_names = self.bulk_data.dtype.names
        header_data = []
        with open(self.file_name) as f:
            for i, line in enumerate(f):
                if i == MesaData.header_names_line - 1:
                    self.header_names = line.split()
                elif i == MesaData.header_names_line:
                    header_data = [eval(datum) for datum in line.split()]
                elif i > MesaData.header_names_line:
                    break
        self.header_data = dict(zip(self.header_names, header_data))
        self.remove_backups()

    
    def data(self, key):
        """Accesses the data and returns a numpy array with the appropriate data

        Accepts a string key, like star_age (for history files) or logRho (for
        profile files) and returns the corresponding numpy array of data for
        that data type. Can also just use the shorthand methods that have the
        same name of the key.

        """
        if self.in_data(key):
            return self.bulk_data[key]
        elif self._log_version(key) is not None:
            return 10**self.bulk_data[self._log_version(key)]
        elif self._ln_version(key) is not None:
            return np.exp(self.bulk_data[self._ln_version(key)])
        elif self._exp10_version(key) is not None:
            return np.log10(self.bulk_data[self._exp10_version(key)])
        elif self._exp_version(key) is not None:
            return np.log(self.bulk_data[self._exp_version(key)])
        else:
            raise KeyError("'" + str(key) + "' is not a valid data type.")

    def header(self, key):
        """Accesses the header, returning a scalar the appropriate data

        Accepts a string key, like version_number and returns the corresponding
        datum for that key. Can also just use the shorthand
        methods that have the same name of the key.

        """

        if not self.in_header(key):
            raise KeyError("'" + str(key) + "' is not a valid header name.")
        return self.header_data[key]

    def is_history(self):
        """Determine if the source file is a history file

        Checks if 'model_number' is a valid key for self.data. If it is, return
        True. Otherwise return False. This is used in determining whether or not
        to cleanse the file of backups and restarts in the MesaData.read_data.

        """
        return 'model_number' in self.bulk_names

    def in_header(self, key):
        """Determine if `key` is an available header data category.

        Checks if string `key` is a valid argument of MesaData.header. Returns
        True if it is, otherwise False

        """
        return key in self.header_names

    def in_data(self, key):
        """Determine if `key` is an available main data category.

        """
        return key in self.bulk_names

    def _log_version(self, key):
        """Determine if the log of the desired value is available and return it.

        If a log_10 version of the value desired is found in the data columns,
        the "logified" name will be returned. Otherwise it will return `None`.

        """
        log_prefixes = ['log_', 'log', 'lg_', 'lg']
        for prefix in log_prefixes:
            if self.in_data(prefix + key):
                return prefix + key

    def _ln_version(self, key):
        """Determine if the ln of the desired value is available and return it.

        If a log_e version of the value desired is found in the data columns,
        the "ln-ified" name will be returned. Otherwise it will return `None`.

        """
        log_prefixes = ['ln_', 'ln']
        for prefix in log_prefixes:
            if self.in_data(prefix + key):
                return prefix + key

    def _exp10_version(self, key):
        """Find if the non-log version of a value is available and return it

        If a non-log version of the value desired is found in the data columns,
        the linear name will be returned. Otherwise it will return `None`.

        """
        
	log_matcher = re.compile('^lo?g_?(.+)')
        matches = log_matcher.match(key)
        if matches is not None:
            groups = matches.groups()
            if self.in_data(groups[0]):
                return groups[0]

    def _exp_version(self, key):
        """Find if the non-ln version of a value is available and return it

        If a non-ln version of the value desired is found in the data columns,
        the linear name will be returned. Otherwise it will return `None`.
	"""
        
	log_matcher = re.compile('^ln_?(.+)')
        matches = log_matcher.match(key)
        if matches is not None:
            groups = matches.groups()
            if self.in_data(groups[0]):
                return groups[0]

    def _any_version(self, key):
        """Determine if `key` can point to a valid data category"""

        return bool(self.in_data(key) or self._log_version(key) or
                    self._ln_version(key) or self._exp_version(key) or
                    self._exp10_version(key))

    def data_at_model_number(self, key, m_num):
        """Return main data at a specific model number (for history files).

        Finds the index i where MesaData.data('model_number')[i] == m_num. Then
        returns MesaData.data(key)[i]. Essentially lets you use model numbers
        to index data.

        """
        return self.data(key)[self.index_of_model_number(m_num)]

    def index_of_model_number(self, m_num):
        """Return index where MesaData.data('model_number') is `m_num`.

        Returns the index i where MesaData.data('model_number')[i] == m_num.

        """
        if not self.is_history():
            raise HistoryError("Can't get data at model number " +
                               "because this isn't a history file")
        index = np.where(self.data('model_number') == m_num)[0]
        if len(index) > 1:
            raise ModelNumberError("Found more than one entry where model " +
                                   "number is " + str(m_num) + " in " +
                                   self.file_name + ". Report this.")
        elif len(index) == 0:
            raise ModelNumberError("Couldn't find any entries with model " +
                                   "number " + str(m_num) + ".")
        elif len(index) == 1:
            return index[0]

    def remove_backups(self, dbg=False):
        """Cleanses a history file of backups and restarts

        If the file is a history file, goes through and ensure that the
        model_number data are monotonically increasing. It removes rows of data
        from all categories if there are earlier ones later in the file.

        """
        if not self.is_history():
            return None
        if dbg:
            print("Scrubbing history...")
        to_remove = []
        for i in range(len(self.data('model_number')) - 1):
            smallest_future = np.min(self.data('model_number')[i + 1:])
            if self.data('model_number')[i] >= smallest_future:
                to_remove.append(i)
        if len(to_remove) == 0:
            if dbg:
                print("Already clean!")
            return None
        if dbg:
            print("Removing {} lines.".format(len(to_remove)))
        self.bulk_data = np.delete(self.bulk_data, to_remove)

    def __getattr__(self, method_name):
        if self._any_version(method_name):
            return self.data(method_name)
        elif self.in_header(method_name):
            return self.header(method_name)
        else:
            raise AttributeError(method_name)

