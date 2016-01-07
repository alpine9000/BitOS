GITVERSION := $(shell git describe --abbrev=41 --dirty --always --tags)
VERSION=-DGITVERSION=\"$(GITVERSION)\"