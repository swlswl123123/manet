markdown=$1
pandoc --number-sections -s -t docx --filter pandoc-crossref --filter pandoc-citeproc --bibliography=bib/references.bib --csl=bib/chinese-gb7714-2005-numeric.csl -o ${markdown%.*}.docx ${markdown%.*}.md
