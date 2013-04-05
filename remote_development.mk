#
# Makefile for remote development
#

HOST       = linux-dev
DIR        = ~/log4cxy
GIT_MIRROR = linux-dev

all clean run profile perf_report: push_mirror
	ssh $(HOST) 'cd $(DIR) && git reset --hard && make $@'

push_mirror:
	git push --mirror $(GIT_MIRROR)

amend:
	git commit -a --amend -C HEAD