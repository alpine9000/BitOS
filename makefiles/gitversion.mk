GITVERSION := $(shell git describe --dirty --always --tags)
VERSION=-DGITVERSION=\"$(GITVERSION)\"