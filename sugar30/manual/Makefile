
#
# Makefile for SUGAR 3.0
#

main-all: user.tex 
	pdflatex user.tex
	bibtex user
	pdflatex user.tex
	pdflatex user.tex

main.pdf: user.tex 
	pdflatex user.tex


# --- Helpers

clean:
	rm -f *.log *.aux *.toc *.bbl *.blg *~

clean-all: clean
	rm -f *.pdf

count:
	wc -l *.tex

