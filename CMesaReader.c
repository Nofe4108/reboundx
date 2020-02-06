//
//  CMesaReader.c
//  
//
//  Created by Noah Ferich on 2/4/20.
//

#include "CMesaReader.h"
#include <stdio.h>
#include <string.h>


struct MesaData {
  
    int header_nmaes_line = 2;
    int bulk_names_line = 6;
    
    char file_name[1000];
    char file_type[4] = ".log"; //Not sure if this will be needed
    double bulk_data[] = {};
    
    
}

/*Structure containing data from a Mesa output file.

Reads a profile or history output file from mesa. Assumes a file with
the following structure:

- line 1: header names
- line 2: header data
- line 3: blank
- line 4: main data names
- line 5: main data values

This structure can be altered by using the class methods
MesaData.set_header_rows and MesaData.set_data_rows.

Parameters
----------
file_name : str, optional
    File name to be read in. Default is 'LOGS/history.data', which works
    for scripts in a standard work directory with a standard logs directory
    for accessing the history data.

Attributes
----------
file_name : str
    Path to file from which the data is read.
bulk_data : numpy.ndarray
    The main data (line 6 and below) in record array format. Primarily
    accessed via the `data` method.
bulk_names : tuple of str
    Tuple of all available data column names that are valid inputs for
    `data`. Essentially the column names in line 4 of `file_name`.
header_data : dict
    Header data (line 2 of `file_name`) in dict format
header_names : list of str
    List of all available header column names that are valid inputs for
    `header`. Essentially the column names in line 1 of `file_name`.*/


