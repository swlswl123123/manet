markdown=$1
lang=$2

if [ -n $lang ]
then
    fig="图"
    tbl="表"
    lst="代码"
    eqn="式"
elif [ $lang = "en" ]
then
    fig="Figure"
    tbl="Table"
    lst="Code"
    eqn="Equation"
fi

sed 's/.svg/.pdf/g' $1 > ${markdown%.*}.pdf.md

## proc fig
sed -i 's/\[@\(fig:[0-9a-zA-Z_]*\)\]/'$fig'\\ref{\1}/g' ${markdown%.*}.pdf.md

## proc tbl
sed -i 's/\[@\(tbl:[0-9a-zA-Z_]*\)\]/'$tbl'\\ref{\1}/g' ${markdown%.*}.pdf.md

## proc sec
sed -i 's/\[@\(sec:[0-9a-zA-Z_]*\)\]/\\ref{\1}/g' ${markdown%.*}.pdf.md

## proc eq

sed -i 's/\[@\(eq:[0-9a-zA-Z_]*\)\]/'$eqn'\\ref{\1}/g' ${markdown%.*}.pdf.md

## proc lst

sed -i 's/\[@\(lst:[0-9a-zA-Z_]*\)\]/'$lst'\\ref{\1}/g' ${markdown%.*}.pdf.md

