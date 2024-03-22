echo "===== benchmark for all maps start ====="

for map_file in ./maps/*.txt; do
    echo "benchmark for $map_file"
    ./PreliminaryJudge -m "$map_file" -l NONE "./tmp/build/main"
done

echo "===== benchmark for all maps done ====="