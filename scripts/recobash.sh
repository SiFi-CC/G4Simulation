# prepare a reconstaruction files for the simulations with different source positions
# look at simbash.sh so that filenames here correspond to the simulated files

export LC_NUMERIC="en_US.UTF-8"
# number of simultaneous processes to perform
N=8

# input/output directory
OUTPUT_DIR=./1d_map_467

(
for sx in `seq -30 0.5 30`
	do
        ((i=i%N)); ((i++==0)) && wait
        echo "sx = ${sx}";
        ./bin/cm_g4reconstruct $OUTPUT_DIR/sim220_170_n1e6_det32_2lay_nowallpetcut31_mask467_70mm_1d_${sx}_0.root $OUTPUT_DIR/matr220_170_n1e6_det32_2lay_nowallpetcut31_mask467_70mm_1d.root -n 200 -o $OUTPUT_DIR/reco220_170_n1e6_det32_2lay_nowallpetcut31_mask467_70mm_1d_${sx}_0.root &>>out.log &

        echo "done"
	done
)