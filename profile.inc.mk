# ===========================
# Profiling
# ===========================
PROFILE_NUM_THREADS     = $(RUN_NUM_THREADS)
PROFILE_REPORT_DIR      = report
PROFILE_FILENAME        = $(PROFILE_REPORT_DIR)/$(LOG4CXY).prof
PROFILE_REPORT_FILENAME = $(PROFILE_REPORT_DIR)/perf_report_$(PROFILE_NUM_THREADS)
GOOGLE_PPROF            = /usr/bin/google-pprof
# ===========================

# ===========================
# Make targets
# ===========================

.PHONY +=  profile perf_report

profile: $(PROFILE_FILENAME)

$(PROFILE_FILENAME): $(LOG4CXY)
	@test -d $(PROFILE_REPORT_DIR) || mkdir -v  $(PROFILE_REPORT_DIR)
	env LD_PRELOAD="/usr/lib/libprofiler.so" CPUPROFILE=$(PROFILE_FILENAME) CPUPROFILE_FREQUENCY=200 \
	./$(LOG4CXY) $(RUN_ARGS)
	test -f $@

perf_report: $(PROFILE_REPORT_FILENAME).txt $(PROFILE_REPORT_FILENAME).pdf

$(PROFILE_REPORT_FILENAME).txt $(PROFILE_REPORT_FILENAME).pdf: $(PROFILE_FILENAME)
	@echo Generate text profile report...
	$(GOOGLE_PPROF) --text $(LOG4CXY) $(PROFILE_FILENAME) | tee $(PROFILE_REPORT_FILENAME).txt | head
	@echo Generate PDF  profile report...
	$(GOOGLE_PPROF) --pdf $(LOG4CXY) $(PROFILE_FILENAME) > $(PROFILE_REPORT_FILENAME).pdf

