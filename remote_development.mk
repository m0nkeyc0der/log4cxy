#
# Makefile for remote development
#

HOST       = 192.168.1.103
DIR        = ~/log4cxy
GIT_MIRROR = linux-dev

RESET_REPO = git reset --hard

all clean run profile perf_report: push_mirror
	ssh $(HOST) 'cd $(DIR) && $(RESET_REPO) && make $@'

push_mirror:
	git push --mirror $(GIT_MIRROR)

amend:
	git commit -a --amend -C HEAD