#
# Makefile for SUGAR 3.0
#

main.ps: user.tex 
	latex user.tex
	bibtex user
	latex user.tex
	latex user.tex
	dvips user.dvi -o 

main.pdf: user.tex 
	pdflatex user.tex
	bibtex user
	pdflatex user.tex
	pdflatex user.tex

html: user.tex
	latex2html -no_math -html_version 3.2,math,table -split +1 user.tex

clean:
	rm -f *.out *.log *.aux *.toc *.bbl *.blg *~

clean-all: clean
	rm -f *.pdf *.ps
