#/bin/bash

if [ $(ls TimeSeries_Port* | wc -l) -gt 0 ]
then
	find . -size 0 -delete
	cat TimeSeries_Port* | grep -v $'^0' | awk '{gbps+=$2; pkts+=$3; pktloss+=$4; i++} END {printf "%.6lf\t%.1lf\t%lf\n", gbps/i,pkts/i,pktloss/i}' | sed -e 's/\./,/g' > sumarized.txt
	echo -n "Media: "
	cat sumarized.txt
	echo -n "Total: "
	cat TimeSeries_Port* | grep -v $'^0' | awk '{gbps+=$2; pkts+=$3; pktloss+=$4; i++} END {printf "%.6lf\t%.1lf\t%lf\n", gbps/(i/'$(ls TimeSeries_Port* | wc -l)'),pkts/(i/'$(ls TimeSeries_Port* | wc -l)'),pktloss/(i/'$(ls TimeSeries_Port* | wc -l)')}' | sed -e 's/\./,/g' 
	cat TimeSeries_Port* | grep -v $'^0' | awk '{gbps+=$2; pkts+=$3; pktloss+=$4; i++} END {printf "%.6lf\t%.1lf\t%lf\n", gbps/(i/'$(ls TimeSeries_Port* | wc -l)'),pkts/(i/'$(ls TimeSeries_Port* | wc -l)'),pktloss/(i/'$(ls TimeSeries_Port* | wc -l)')}' | sed -e 's/\./,/g' >> sumarized.txt
	mv TimeSeries_Port* data/.
else
	cat sumarized.txt
fi
