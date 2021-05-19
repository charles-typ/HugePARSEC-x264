source env.sh

parsecmgmt -a uninstall -p x264 -c gcc-pthreads
parsecmgmt -a build -p x264 -c gcc-pthreads
#parsecmgmt -a run -p parsec.x264 -i native -c gcc-pthreads  -s "perf stat -e dTLB-load-misses,dTLB-store-misses,dtlb_load_misses.walk_active,dtlb_store_misses.walk_active,mem_inst_retired.all_loads,mem_inst_retired.all_stores,mem_inst_retired.stlb_miss_loads,mem_inst_retired.stlb_miss_stores,cycles"
