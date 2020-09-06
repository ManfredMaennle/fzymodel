# fzymodel
Fully automated creation of Sugeno type neuro-fuzzy models by deep learning based on input/output data. Examples are provided for systems identification and time series modeling (static and dynamic systems).

Please address comments to fzymodel@maennle.org

The tool set includes
 - `fzynorml`: normalization of measured data (value space between approximately [-0.5; 0.5])
 - `fzymkdat`: prepare data for modelling / system identification: learn / validation / test data sets in case of dynamic systems: create regressors
 - `fzymodel`: system identification: iteratively refining TSK fuzzy models
 - `fzyestim`: estimation run (one-step prediction, no feedback)
 - `fzysimul`: simulation run (multi-step prediction with feedback)
 - `fzy2sets`: create LaTeX file describing the fuzzy model

The main innovation of this tool set is the iterative, data based approach for creating and refining TSK fuzzy models using various optimization algorithms from numerics and neural network training (deep learning). Descriptions can be found in my scientific papers, cf. to the `.\examples` folder, e.g. the [structure search and parameter learning algorithm.](https://github.com/ManfredMaennle/fzymodel/blob/master/examples/gauss_19990901_eufit99.pdf)


## EXAMPLES

 - `gauss_2dim`: A twodimensional static function mapping. It is a good eaxample to visually demonstrate how the modelling works. It is recommended to start playing with this example.
 - `tank`: System identification of a tank as an example of simulated dynamic system.
 - `turbo`: System identification of a turbo charger as an example of a real-world dynamic system.
Please refer to the pubished papers (pfd documents) for more details.


## BINARIES

Win32 binaries in this distribution are compiled using GNU C++ from the MinGW 32bit package. For installation just unpack the zip file. Paths in the examples folder are relative to the `bin` folder.


## SOURCES

Full C++ sources are provided in the `src` folder. The `include` directory contains used packages such as TNT 1.2.6 (template numerical toolkit). For recompilation, check the `Makefile` and start `make all`. The sources should compile with GNU gcc version 4.6.2 or higher. The file `fzymodeldoc.pdf` in the `doc` folder contains some minimal source code documentation.

If you have questions, please send me a message at fzymodel@maennle.org

![image](https://github.com/ManfredMaennle/fzymodel/blob/master/example_gauss_short_description.png)
