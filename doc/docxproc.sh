markdown=$1
sed 's/.svg/.png/g' $1 > ${markdown%.*}.docx.md