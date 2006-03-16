sed 's,TRACE([ \t]*\(".*"\),TRACE( _T(\1),g' $1 > $1.out
sed 's-TRACEUSER([ \t]*\(.*\),[ \t]*\(".*"\)-TRACEUSER( \1, _T(\2)-g' $1.out > $1
rm $1.out
