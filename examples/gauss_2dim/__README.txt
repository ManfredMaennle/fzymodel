
gauss_2dim: example of a twodimensional bell-shaped function to
            test modeling (function approximation) algorithms

You need to have installed: perl, gnuplot, fzymodel, fzymkdat, fzy2sets, latex.


### scripts to start in the following order
_go_mod.sh: create training data, run fzymodel to build a series of models
            needs: createdat_gauss.pl, fzymkdat, fzymodel

_show_out.sh: plot all model outputs as .eps and .png
            needs: show_out.pl, gnuplot

_show_fzymodels.sh: plot fuzzy model descriptions as .dvi and .pdf
            needs: fzy2sets, gnuplot, latex; all_models.tex for all_models.pdf


(optionally: _clean.sh: delete all created files except .png and .pdf result files)
(optionally: _cleanall.sh: delete all created files)


### latex settings
tex_common.tex: latex macros and settings for fzymodel printing
tex_modheader.tex: header for each fzymodel_xxx.pdf
all_fzymodels.tex: master latex file for all_fzymodels.pdf

You can directly include a latex model desciption by \input{mod_xxx.tex} in any LaTeX file,
if you have \input{tex_common} before.

 
