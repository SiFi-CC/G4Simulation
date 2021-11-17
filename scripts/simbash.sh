# prepare a simulations with different source positions
# please, change simulation parameters according to your needs

# use 'dot' as decimal separator 
export LC_NUMERIC="en_US.UTF-8"

# number of simultaneous processes to perform
N=4

# output directory
OUTPUT_DIR=./1d_map_467

(
# source positions are defined in the loop: `seq <first_value> <step> <last_value>`
for sx in `seq -30 0.5 30`
	do
            ((i=i%N)); ((i++==0)) && wait
        echo "sx = ${sx}";
        ./cmd/custom_simulation $OUTPUT_DIR/sim220_170_n1e6_det32_2lay_nowallpetcut31_mask467_70mm_1d_${sx}_0.root -det 220:32:1.3 -nlay 2 -mask 467:170:70:20 -n  1000000 -masktype nowallpetcut -cut 31 -sourceBins 70:100 -1d -source ${sx}:0 &>>out.log &
        
        echo "done"
	done
)