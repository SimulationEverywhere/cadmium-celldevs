# C++ Cadmium output parser

This parser takes the output log from Cadmium Cell-DEVS simulation and converts it to
a suitable input for the ARSLab SimpleViewer API.

Compile the parser simply with:
$ g++ -o parser Parser_Cadmium_CellDEVS.cpp

The parser is invoked as follows:
- Windows: $./parser.exe input_messages_log.txt model_name number_of_ports output_folder
- Linux: $./parser input_messages_log.txt model_name number_of_ports output_folder

  - "input_messages_log.txt" is the relative path to the Cadmium Cell-DEVS output log;
  - "model_name" is a parameter to specify the name to be shown in the visualizer and has no other effect.
  - "number_of_ports" is the number of values that are concatenated in each cell state.
  - "output_folder" is the relative path of the folder that the parser creates to output the resulting files.

The parser generates 3 output files and stores them in the output_folder: options.json, simulation.json and transitions.csv.
- "options.json" specifies different settings for the visualizer as the playback speed and the number of columns to draw, in order to see the output for the different ports.
- "transitions.csv" holds the converted model output messages.
- "simulation.json" contains data regarding the type of model being executed, the size of the model and a palette field to colour the cell states. An example RGB palette is pre-loaded in the parser, but the user can replace it for the right colours to represent the states.

Once the 3 files are generated, enter the visualizer site (https://staubibr.github.io/arslab-prd/app-simple/index.html), load the files in the visualizer with the "Load Simulation" button in the left, or just drag and drop the files in the center region of the screen, and press the wide "Load Simulation" button below. The cell layout for the simulation model and the playback controls should then be shown.
