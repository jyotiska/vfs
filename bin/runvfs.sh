###################
# Shell script to run an VFS executable file against a set of interactions scripts
#
# Usage: runvfs.sh executablefilepath datafilepath
#
#    executablefilepath - The name of executable fle including the path (e.g. bin/vfs_group01)
#    datafilepath - The path to the directory that contains the .in and .out files
###################

EXE=$1
DATAFILEPATH=$2
RPTFILE=$EXE.rpt
TEMPOUTFILE=/tmp/tempvfs.out

if [ $# -ne 2 ]
then
	echo Usage: runvfs.sh executablefilepath datafilepath
	exit 1
fi

if [ ! -x $EXE ]
then
	echo Cannot execute $EXE
	exit 2
fi

if [ ! -d $DATAFILEPATH ]
then
	echo $DATAFILEPATH is not a directory
	exit 3
fi

# Run the program against each data file

rm -f $RPTFILE

echo ================================================================== >> $RPTFILE
echo Report file for $EXE generated on `date`				>> $RPTFILE
echo ==================================================================	>> $RPTFILE

score=0
for infile in $DATAFILEPATH/*.in
do
	$EXE $infile > $TEMPOUTFILE
	EXPECTEDOUTFILE=$DATAFILEPATH/`basename $infile .in`.out

	linecount=0

	set `wc -l $infile`
	numcommands=$1
	numpass=0
	while read actualoutput
	do
		linecount=`expr $linecount + 1`

		#remove all spaces from the outputline
		x=`echo $actualoutput | sed -e 's/ //g'`
		actualoutput=$x

		#echo tail n +$linecount $EXPECTEDOUTFILE s head -n 1
		expectedoutput=`tail -n +$linecount $EXPECTEDOUTFILE | head -n 1`

		#remove all spaces from the outputline
		x=`echo $expectedoutput | sed -e 's/ //g'`
		expectedoutput=$x

		if [ "$expectedoutput" = "$actualoutput" ]
		then
			# add 1 to the score
			numpass=`expr $numpass + 1`
			score=`expr $score + 1`
		fi
	done < $TEMPOUTFILE
	echo Testresult:$EXE:$infile:$numpass:$numcommands >> $RPTFILE
done

set `wc -l $DATAFILEPATH/*.in | tail -n 1`
maxmarks=$1

# Report format: exename:scoreobtained:maximumscore
echo Finalscore:$EXE:$score:$maxmarks >> $RPTFILE

