#
# Makefile for SUGAR 3.0
#

main.pdf: main.tex 
	pdflatex main.tex
	bibtex main
	pdflatex main.tex
	pdflatex main.tex

main.ps: main.tex 
	latex main.tex
	bibtex main
	latex main.tex
	latex main.tex
	dvips main.dvi -o main.ps

clean:
	rm -f *.log *.aux *.toc *.bbl *.blg *~

clean-all: clean
	rm -f *.pdf
