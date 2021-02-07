svgs=`ls *.svg`
pdfs=`ls *.svg|sed 's/.svg/.pdf/g'`
pngs=`ls *.svg|sed 's/.svg/.png/g'`

%.pdf:%.svg
	./imgconv.sh $<

%.png:%.svg
	./svg2png.sh $<

%.docx:%.md
	make $(pngs)
	./docxproc.sh $<
	pandoc --number-sections -s -t docx --filter pandoc-crossref --filter pandoc-citeproc --bibliography=bib/references.bib --csl=bib/chinese-gb7714-2005-numeric.csl -o $@ $@.md

%.pdf:%.md eisvogel.latex
	make $(pdfs)
	./latexproc.sh $<
	pandoc --template eisvogel.latex --columns=50 --number-sections --listings --top-level-division="chapter" --bibliography=bib/references.bib --csl=bib/chinese-gb7714-2005-numeric.csl -t latex --latex-engine=xelatex -o $@ $@.md
	rm -rvf $@.md