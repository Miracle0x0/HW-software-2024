echo "benchmark for all maps"

for map_file in ./maps/*.txt; do
    echo "benchmark for $map_file"
    ./PreliminaryJudge -m "$map_file" -l NONE "./tmp/build/main"
done
