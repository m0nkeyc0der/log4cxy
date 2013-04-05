# ===========================
# Profiling
# ===========================
PROFILE_NUM_THREADS     = 16
PROFILE_NUM_MESSAGES    = 100000
PROFILE_REPORT_DIR      = report
PROFILE_FILENAME        = $(PROFILE_REPORT_DIR)/$(LOG4CXY).prof
PROFILE_REPORT_FILENAME = $(PROFILE_REPORT_DIR)/perf_report_$(PROFILE_NUM_THREADS)x$(PROFILE_NUM_MESSAGES)
GOOGLE_PPROF            = /usr/bin/google-pprof
# ===========================

# ===========================
# Make targets
# ===========================

.PHONY +=  profile perf_report

profile: $(PROFILE_FILENAME)

$(PROFILE_FILENAME): $(LOG4CXY) $(PROFILE_REPORT_DIR)
	env LD_PRELOAD="/usr/lib/libprofiler.so" CPUPROFILE=$(PROFILE_FILENAME) CPUPROFILE_FREQUENCY=400 \
	./$(LOG4CXY) $(PROFILE_NUM_MESSAGES) $(PROFILE_NUM_THREADS)
	test -f $@

$(PROFILE_REPORT_DIR):
	mkdir -v $(PROFILE_REPORT_DIR)

perf_report: $(PROFILE_FILENAME) $(PROFILE_REPORT_DIR)
	@echo Generate text profile report...
	$(GOOGLE_PPROF) --text $(LOG4CXY) $(PROFILE_FILENAME) | tee $(PROFILE_REPORT_FILENAME).txt | head
	@echo Generate PDF  profile report...
	$(GOOGLE_PPROF) --pdf $(LOG4CXY) $(PROFILE_FILENAME) > $(PROFILE_REPORT_FILENAME).pdf

