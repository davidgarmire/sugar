
#
# Makefile for SUGAR 3.0
#

include ../make.inc

# -----------------------------------------------------------------

main-all: main.tex 
	pdflatex main.tex
	bibtex main
	pdflatex main.tex
	pdflatex main.tex

main.pdf: main.tex 
	pdflatex main.tex


# --- Helpers

clean:
	rm -f *.log *.aux *.toc *.bbl *.blg *~

clean-all: clean
	rm -f *.pdf

count:
	wc -l *.tex

