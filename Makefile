svgs=`ls *.svg`
pdfs=`ls *.svg|sed 's/.svg/.pdf/g'`
pngs=`ls *.svg|sed 's/.svg/.png/g'`
ant:
	op_mkema -m directive
	mv -vf directive.dev32.i0.em.x mkant.exe
	mkant

network:
	op_mkema -m 724network
	mv -vf 724network.dev32.i0.em.x mknet.exe
	
%.pdf:%.svg
	./imgconv.sh $<

%.png:%.svg
	./svg2png.sh $<

pdf:
	make $(pdfs)
	./latexproc.sh 总体方案.md
	cp eisvogel.tex eisvogel.latex
	pandoc --template eisvogel.latex --columns=50 --number-sections --listings --top-level-division="chapter" -t latex --latex-engine=xelatex --bibliography=references.bib --csl=chinese-gb7714-2005-numeric.csl  -o 总体方案.pdf 总体方案.latex.md

tex:
	make $(pdfs)
	./latexproc.sh 总体方案.md
	cp eisvogel.tex eisvogel.latex
	pandoc --template eisvogel.latex --columns=50 --number-sections --listings --top-level-division="chapter" -t latex --latex-engine=xelatex --bibliography=references.bib --csl=chinese-gb7714-2005-numeric.csl  -o 总体方案.tex 总体方案.latex.md

html:
	pandoc --number-sections -s -t html --filter pandoc-crossref --filter pandoc-citeproc --mathjax --bibliography=references.bib --csl=chinese-gb7714-2005-numeric.csl --template=tufte.html5 --css=pandoc.css  -o ~/Documents/总体方案.html 总体方案.md
	cp *.svg ~/Documents
	cp *.png ~/Documents
	cp *.jpg ~/Documents
	cp *.gif ~/Documents
	cp *.css ~/Documents
docx:
	make $(pngs)
	./docxproc.sh 总体方案.md
	pandoc --number-sections -s -t docx --filter pandoc-crossref --filter pandoc-citeproc --bibliography=references.bib --csl=chinese-gb7714-2005-numeric.csl -o ~/Documents/总体方案.docx 总体方案.md

demo:
	pandoc --number-sections -s -t html --filter pandoc-crossref -o ~/Documents/output.html demo.md

router:router.cpp node.cpp router_test.cpp
	g++ -g -DTEST -I. $^ -o $@
transfer_queue:transfer_queue.cpp packet.cpp queue_test.cpp
	g++ -g -I. $^ -o $@

balloc:balloc.cpp balloc.test.cpp
	g++ -g -I. $^ -o $@